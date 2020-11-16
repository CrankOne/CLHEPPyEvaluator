import unittest
import CLHEPEvaluator

class TestVariableDefinition(unittest.TestCase):
    def setUp(self):
        self.e = CLHEPEvaluator.Evaluator()

    def test_variable_definition(self):
        self.e.set_variable("twelve", 12)
        self.assertFalse(self.e.status())
        self.assertTrue(self.e.find_variable("twelve"))
        self.assertEqual( self.e.evaluate("twelve"), 12., "Failed to define var" )
        self.assertFalse(self.e.status())
        self.e.remove_variable("twelve")
        self.assertFalse(self.e.status())
        self.assertFalse(self.e.find_variable("twelve"))

    def test_repeated_definition_warning(self):
        self.e.set_variable("overriden", 24)
        self.assertFalse(self.e.status())
        self.assertEqual(self.e.evaluate("overriden"), 24., "Failed to define var")
        self.assertFalse(self.e.status())
        self.e.set_variable("overriden", 42)
        self.assertEqual(self.e.status(), CLHEPEvaluator.WARNING_EXISTING_VARIABLE)
        self.assertEqual(self.e.evaluate("overriden"), 42., "Failed to re-define var")

    def test_arithmetics(self):
        self.e.set_variable("a", 2)
        self.e.set_variable("b", 8)
        self.assertEqual( self.e.evaluate("(a**b + b)/12"), 22.
                        , "Failed to perform simple arithmetics")

    def test_error_reporting(self):
        self.e.evaluate("nonexisting(12)")
        self.assertEqual(self.e.status(), CLHEPEvaluator.ERROR_UNKNOWN_FUNCTION)
        self.assertTrue('function' in self.e.error_name())  # well, sorta...
        self.e.evaluate("12 + 11/nonexisting")
        self.assertEqual(self.e.status(), CLHEPEvaluator.ERROR_UNKNOWN_VARIABLE)
        self.assertEqual(8, self.e.error_position())
        # ... there are a lot of others, but wrapper should already expose
        # errors if there are any

class TestDefaults(unittest.TestCase):
    def setUp(self):
        self.e = CLHEPEvaluator.Evaluator()

    def test_default_system_of_units(self):
        # Expected: none units defined by default
        self.assertFalse( self.e.evaluate("15*cm") )
        self.assertEqual( self.e.status(), CLHEPEvaluator.ERROR_UNKNOWN_VARIABLE )
        self.e.set_system_of_units()
        # Expected: at least standard length units available
        self.assertEqual(self.e.evaluate("1000*m"), self.e.evaluate("1*km"))
        self.e.clear()
        self.assertFalse( self.e.evaluate("1*km") )
        self.assertEqual( self.e.status(), CLHEPEvaluator.ERROR_UNKNOWN_VARIABLE )

    def test_default_math(self):
        self.assertFalse( self.e.evaluate("sqrt(5+11)") )
        self.assertEqual( self.e.status(), CLHEPEvaluator.ERROR_UNKNOWN_FUNCTION )
        self.e.set_std_math()
        self.assertEqual( 4., self.e.evaluate("sqrt(5+11)") )

if __name__ == '__main__':
    unittest.main()
