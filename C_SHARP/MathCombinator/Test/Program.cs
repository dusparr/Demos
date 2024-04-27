using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Test
{
    class Program
    {
        static void Main(string[] args)
        {
            for(uint i = 0; i <= 100; i++ )
            {
                FizzBuzz(i);
            }


        }

        static void FizzBuzz(uint num)
        {
            if(num == 0)
            {
                Console.WriteLine(num);
                return;
            }
            bool notFB = true;
            if(num % 3 == 0)
            {
                notFB = false;
                Console.Write("Fizz");
            }
            if(num % 5 == 0)
            {
                notFB = false;
                Console.Write("Buzz");
            }
            if(notFB)
            {
                Console.Write(num);
            }
            Console.Write("\n");
        }
    }
}
