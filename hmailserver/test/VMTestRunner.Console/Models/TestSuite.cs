namespace VMTestRunner.Console
{
   public enum TestSuite
   {
      /// <summary>
      /// The regression suite. This is what a normal run executes.
      /// </summary>
      RegressionTests,

      /// <summary>
      /// The volume tests. Long running, so only used by runs selected by name.
      /// </summary>
      VolumeTests,
   }
}
