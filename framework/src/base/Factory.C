//* This file is part of the MOOSE framework
//* https://mooseframework.inl.gov
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "Factory.h"
#include "Registry.h"
#include "InfixIterator.h"
#include "InputParameterWarehouse.h"
#include "FEProblemBase.h"
// Just for testing...
#include "Diffusion.h"

Factory::Factory(MooseApp & app) : _app(app) {}

Factory::~Factory() {}

void
Factory::reg(std::shared_ptr<RegistryEntryBase> obj)
{
  const std::string obj_name = obj->name();
  const std::string & label = obj->_label;
  const std::string & deprecated_time = obj->_deprecated_time;
  const std::string & replacement_name = obj->_replaced_by;
  const std::string & file = obj->_file;
  const int line = obj->_line;

  // do nothing if we have already added this exact object before
  auto key = std::make_pair(label, obj_name);
  if (_objects_by_label.find(key) != _objects_by_label.end())
    return;

  /*
   * If _registerable_objects has been set the user has requested that we only register some
   * subset
   * of the objects for a dynamically loaded application. The objects listed in *this*
   * application's
   * registerObjects() method will have already been registered before that member was set.
   *
   * If _registerable_objects is empty, the factory is unrestricted
   */
  if (_registerable_objects.empty() ||
      _registerable_objects.find(obj_name) != _registerable_objects.end())
  {
    if (_name_to_object.find(obj_name) != _name_to_object.end())
      mooseError("Object '" + obj_name + "' registered from multiple files: ",
                 file,
                 " and ",
                 _name_to_line.getInfo(obj_name).file());
    _name_to_object[obj_name] = obj;
    _objects_by_label.insert(key);
  }
  _name_to_line.addInfo(obj_name, file, line);

  if (!replacement_name.empty())
    _deprecated_name[obj_name] = replacement_name;
  if (!deprecated_time.empty())
    _deprecated_time[obj_name] = parseTime(deprecated_time);

  // TODO: Possibly store and print information about objects that are skipped here?
}

InputParameters
Factory::getValidParams(const std::string & obj_name) const
{
  const auto it = _name_to_object.find(obj_name);

  // Check if the object is registered
  if (it == _name_to_object.end())
    reportUnregisteredError(obj_name);

  // Print out deprecated message, if it exists
  deprecatedMessage(obj_name);

  // Return the parameters
  auto params = it->second->buildParameters();
  params.addPrivateParam("_moose_app", &_app);

  return params;
}

std::unique_ptr<MooseObject>
Factory::createUnique(const std::string & obj_name,
                      const std::string & name,
                      const InputParameters & parameters,
                      THREAD_ID tid /* =0 */,
                      bool print_deprecated /* =true */)
{
  if (print_deprecated)
    mooseDeprecated("Factory::create() is deprecated, please use Factory::create<T>() instead");

  // Build the parameters that are stored in the InputParameterWarehouse for this
  // object, set a few other things and do a little error checking
  auto & warehouse_params = initialize(obj_name, name, parameters, tid);

  // call the function pointer to build the object
  _currently_constructing.push_back(&warehouse_params);
  auto obj = _name_to_object.at(obj_name)->build(warehouse_params);
  _currently_constructing.pop_back();

  finalize(obj_name, *obj);

  return obj;
}

std::shared_ptr<MooseObject>
Factory::create(const std::string & obj_name,
                const std::string & name,
                const InputParameters & parameters,
                THREAD_ID tid /* =0 */,
                bool print_deprecated /* =true */)
{
  std::shared_ptr<MooseObject> object =
      createUnique(obj_name, name, parameters, tid, print_deprecated);

  if (auto fep = std::dynamic_pointer_cast<FEProblemBase>(object))
    _app.actionWarehouse().problemBase() = fep;

  return object;
}

void
Factory::releaseSharedObjects(const MooseObject & moose_object, THREAD_ID tid)
{
  _app.getInputParameterWarehouse().removeInputParameters(moose_object, tid, {});
}

void
Factory::restrictRegisterableObjects(const std::vector<std::string> & names)
{
  _registerable_objects.insert(names.begin(), names.end());
}

std::time_t
Factory::parseTime(const std::string t_str)
{
  // The string must be a certain length to be valid
  if (t_str.size() != 16)
    mooseError("The deprecated time not formatted correctly; it must be given as mm/dd/yyyy HH:MM");

  // Store the time, the time must be specified as: mm/dd/yyyy HH:MM
  std::time_t t_end;
  struct tm * t_end_info;
  time(&t_end);
  t_end_info = localtime(&t_end);
  t_end_info->tm_mon = std::atoi(t_str.substr(0, 2).c_str()) - 1;
  t_end_info->tm_mday = std::atoi(t_str.substr(3, 2).c_str());
  t_end_info->tm_year = std::atoi(t_str.substr(6, 4).c_str()) - 1900;
  t_end_info->tm_hour = std::atoi(t_str.substr(11, 2).c_str()) + 1;
  t_end_info->tm_min = std::atoi(t_str.substr(14, 2).c_str());
  t_end_info->tm_sec = 0;
  t_end = mktime(t_end_info);
  return t_end;
}

void
Factory::deprecatedMessage(const std::string obj_name) const
{
  const auto time_it = _deprecated_time.find(obj_name);

  // If the object is not deprecated return
  if (time_it == _deprecated_time.end())
    return;

  // If the message has already been printed, return
  if (_deprecated_types.count(obj_name))
    return;
  _deprecated_types.emplace(obj_name);

  // We dont need a backtrace on this, this is user-facing
  const auto current_show_trace = Moose::show_trace;
  Moose::show_trace = false;

  // Get the current time
  std::time_t now;
  time(&now);

  // Get the stop time
  std::time_t t_end = time_it->second;

  // Message storage
  std::ostringstream msg;

  const auto name_it = _deprecated_name.find(obj_name);

  // Expired object
  if (now > t_end)
  {
    msg << "***** Invalid Object: " << obj_name << " *****\n";
    msg << "Expired on " << ctime(&t_end);

    // Append replacement object, if it exists
    if (name_it != _deprecated_name.end())
      msg << "Update your application using the '" << name_it->second << "' object";

    // Produce the error message
    mooseDeprecationExpired(msg.str());
  }

  // Expiring object
  else
  {
    // Build the basic message
    msg << "Deprecated Object: " << obj_name << "\n";
    msg << "This object will be removed on " << ctime(&t_end);

    // Append replacement object, if it exists
    if (name_it != _deprecated_name.end())
      msg << "Replace " << obj_name << " with " << name_it->second;

    // Produce the error message
    mooseDeprecated(msg.str());
  }
  Moose::show_trace = current_show_trace;
}

void
Factory::reportUnregisteredError(const std::string & obj_name) const
{
  std::ostringstream oss;
  std::set<std::string> paths = _app.getLoadedLibraryPaths();

  oss << "A '" + obj_name + "' is not a registered object.\n";

  if (!paths.empty())
  {
    oss << "\nWe loaded objects from the following libraries and still couldn't find your "
           "object:\n\t";
    std::copy(paths.begin(), paths.end(), infix_ostream_iterator<std::string>(oss, "\n\t"));
    oss << '\n';
  }

  oss << "\nIf you are trying to find this object in a dynamically loaded library, make sure that\n"
         "the library can be found either in your \"Problem/library_path\" parameter or in the\n"
         "MOOSE_LIBRARY_PATH environment variable.";

  mooseError(oss.str());
}

std::vector<std::string>
Factory::getConstructedObjects() const
{
  std::vector<std::string> list;
  for (const auto & name : _constructed_types)
    list.push_back(name);
  return list;
}

const InputParameters *
Factory::currentlyConstructing() const
{
  return _currently_constructing.size() ? _currently_constructing.back() : nullptr;
}

FileLineInfo
Factory::getLineInfo(const std::string & name) const
{
  return _name_to_line.getInfo(name);
}

void
Factory::associateNameToClass(const std::string & name, const std::string & class_name)
{
  _name_to_class[name] = class_name;
}

std::string
Factory::associatedClassName(const std::string & name) const
{
  auto it = _name_to_class.find(name);
  if (it == _name_to_class.end())
    return "";
  else
    return it->second;
}

InputParameters &
Factory::initialize(const std::string & type,
                    const std::string & name,
                    const InputParameters & from_params,
                    const THREAD_ID tid)
{
  // Pointer to the object constructor
  const auto it = _name_to_object.find(type);

  // Check if the object is registered
  if (it == _name_to_object.end())
    reportUnregisteredError(type);

  // Print out deprecated message, if it exists
  deprecatedMessage(type);

  // Create the actual parameters object that the object will reference
  InputParameters & params =
      _app.getInputParameterWarehouse().addInputParameters(name, from_params, tid, {});

  // Add the hit node from the action if it isn't set already (it might be set
  // already because someone had a better option than just the action)
  // If it isn't set, it typically means that this object was created by a
  // non-MooseObjectAction Action
  if (!params.getHitNode() || params.getHitNode()->isRoot())
    if (const auto hit_node = _app.getCurrentActionHitNode())
      params.setHitNode(*hit_node, {});

  // Set the _type parameter
  params.set<std::string>("_type") = type;

  // Check to make sure that all required parameters are supplied
  params.finalize(name);

  // register type name as constructed
  _constructed_types.insert(type);

  // add FEProblem pointers to object's params object
  if (_app.actionWarehouse().problemBase())
    _app.actionWarehouse().problemBase()->setInputParametersFEProblem(params);

  return params;
}

void
Factory::finalize(const std::string & type, const MooseObject & object)
{
  // Make sure no unexpected parameters were added by the object's constructor or by the action
  // initiating this create call.  All parameters modified by the constructor must have already
  // been specified in the object's validParams function.
  InputParameters orig_params = getValidParams(type);
  const auto & object_params = object.parameters();
  if (orig_params.n_parameters() != object_params.n_parameters())
  {
    std::set<std::string> orig, populated;
    for (const auto & it : orig_params)
      orig.emplace(it.first);
    for (const auto & it : object_params)
      populated.emplace(it.first);

    std::set<std::string> diff;
    std::set_difference(populated.begin(),
                        populated.end(),
                        orig.begin(),
                        orig.end(),
                        std::inserter(diff, diff.begin()));

    if (!diff.empty())
    {
      std::stringstream ss;
      for (const auto & name : diff)
        ss << ", " << name;
      object.mooseError("Attempted to set unregistered parameter(s):\n    ", ss.str().substr(2));
    }
  }
}
