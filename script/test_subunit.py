import subunit

class OCLTests(subunit.ExecTestCase):
    """Run the C Tests"""

    def util_test_group(self):
        """../build/test/util_tests"""
