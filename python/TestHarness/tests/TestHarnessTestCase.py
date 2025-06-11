#* This file is part of the MOOSE framework
#* https://mooseframework.inl.gov
#*
#* All rights reserved, see COPYRIGHT for full restrictions
#* https://github.com/idaholab/moose/blob/master/COPYRIGHT
#*
#* Licensed under LGPL 2.1, please see LICENSE for details
#* https://www.gnu.org/licenses/lgpl-2.1.html

import os
import unittest
import tempfile
import re
import json
import typing
from contextlib import nullcontext
from TestHarness import TestHarness
from unittest.mock import patch
from dataclasses import dataclass

MOOSE_DIR = os.getenv('MOOSE_DIR')
TEST_DIR = os.path.join(MOOSE_DIR, 'test')

class TestHarnessTestCase(unittest.TestCase):
    """
    TestCase class for running TestHarness commands.
    """

    @dataclass
    class RunTestsResult:
        # On screen output
        output: str = ''
        # JSON results
        results: typing.Optional[dict] = None
        # TestHarness that was ran
        harness: typing.Optional[TestHarness] = None

    def runTests(self, *args,
                 tmp_output: bool = True,
                 no_capabilities: bool = True,
                 capture_results: bool = True,
                 exit_code: int = 0) -> RunTestsResult:
        argv = ['unused'] + list(args) + ['--term-format', 'njCst']
        if no_capabilities:
            argv += ['--no-capabilities']

        result = self.RunTestsResult()

        def wrapped_print(*args, **kwargs):
            end = kwargs.get('end', '\n')
            values = [f'{v}' for v in args]
            result.output += " ".join(values) + end

        with patch("builtins.print", wraps=wrapped_print):
            context = tempfile.TemporaryDirectory if tmp_output else nullcontext
            with context() as c:
                if tmp_output:
                    argv += ['-o', c]
                cwd = os.getcwd()
                os.chdir(TEST_DIR)
                try:
                    result.harness = TestHarness.build(argv, None, os.getenv('MOOSE_DIR'))
                    result.harness.findAndRunTests()
                except SystemExit as e:
                    self.assertEqual(e.code, exit_code)
                    return result
                finally:
                    os.chdir(cwd)

                self.assertEqual(result.harness.error_code, exit_code)
                if capture_results:
                    with open(result.harness.options.results_file, 'r') as f:
                        result.results = json.loads(f.read())

        return result

    def checkStatus(self, harness: TestHarness,
                    passed: int = 0,
                    skipped: int = 0,
                    failed: int = 0):
        """
        Make sure the TestHarness status line reports the correct counts.
        """
        self.assertEqual(harness.num_passed, passed)
        self.assertEqual(harness.num_skipped, skipped)
        self.assertEqual(harness.num_failed, failed)
