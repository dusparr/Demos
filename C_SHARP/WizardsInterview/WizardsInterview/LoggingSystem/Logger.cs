using System.IO;

namespace CardData
{
    public class Logger
    {
        //this entire class should be hoisted to its own logger setup, but what are you gonna write in 10 minutes?
        static bool write = true;
        static int call = 0;
        static StreamWriter sw = null;

        public static void log(string s, bool isError = false)
        {
            if (sw != null)
            {
                if (isError || write)
                {
                    sw.WriteLine($@"CALL{call++}:{s}"); //call is so that multi-line output is simpler to read
                }
            }
        }
    }

}
// VERSION DETAILS: C#InterviewPreScreen v1.1 2016-05-17