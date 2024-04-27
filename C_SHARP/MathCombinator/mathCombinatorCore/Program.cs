///Dusty Parrott
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml.Linq;
using System.Runtime.CompilerServices;

[assembly: InternalsVisibleTo("basicUnit")]

//instead of logging, in most cases I am just writing to console, though logging should be added at a later date
//the two exception are throws that need to be handled; file extension not known and attempting to print unevaluated data
//though the latter of the two could evaluate the data ahead of time
//but I left it this way under the assumption that this may end up being used in a thread based system
//where evaulation may be computationaly bound
//I also left, as practice for the reader, a way to add trinary or unary operations in pretty simply
//you would have to add the appropriate class/function combination
//the dictionary for the evaulators
//and add in how it would be read by whatever system you want
//As I believe that it goes beyond the scope of the document. It would be a place where communication between
//automation and the product owner would take place as to the exact way that this should be handled
//I am also specifically trying not to use inheritance in this as it has some weird pitfalls for extensibility

namespace MathCombinator
{
    public class Logger
    {
        //this entire class should be hoisted to its own logger setup, but what are you gonna write in 10 minutes?
        static bool write = true;
        static int call = 0;

        public static void log(string s, bool isError = false)
        {
            if (isError || write)
            {
                Console.WriteLine($@"CALL{call++}:{s}"); //call is so that multi-line output is simpler to read
            }
        }
    }
    class Program
    {
        static void Main(string[] args)
        {
            if (args.Length < 1)
            {
                Reader reader = new Reader();
                reader.AddFile(@"math.xml"); // should allow you to pass this in if you want, 
                reader.GetDataFromFiles();
                MathCombinator mathCombinator = new MathCombinator(reader.getDataToEvaluate());
                mathCombinator.EvaluateData();
                Logger.log($@"Printing Data");
                Logger.log($@"");
                mathCombinator.printData();
            }
            else
            {
                Reader reader = new Reader();
                foreach (string arg in args)
                {
                    reader.AddFile(arg);
                }
                reader.GetDataFromFiles();
                MathCombinator mathCombinator = new MathCombinator(reader.getDataToEvaluate());
                mathCombinator.EvaluateData();
                Logger.log($@"Printing Data");
                Logger.log($@"");
                mathCombinator.printData();
            }
            Console.ReadLine();
        }
    }
    public class FileToRead
    {
        public string filename;
        Action<string> Action;
        public void setEvalFunction(Action<string> func)
        {
            Action = func;
        }
        public FileToRead(string name)
        {
            filename = name;
        }
        public static implicit operator string(FileToRead d) => d.filename;
        public static implicit operator FileToRead(string d)
        {
            return new FileToRead(d);
        }
        public void Evaluate()
        {
            Logger.log($@"Evaulating Function for file {filename}");
            Action.Invoke(filename);
        }

    }
    public class Reader
    {
        Dictionary<string, Action<string>> FileTypesExtensions = new Dictionary<string, Action<string>>();
        List<FileToRead> files = new List<FileToRead>();
        List<BinaryData> BinaryDataToEvaulate = new List<BinaryData>();

        public Reader()
        {
            DefineTypes();
        }
        public Reader(List<string> files)
        {
            DefineTypes();
            foreach (string file in files)
            {
                AddFile(file);
            }
        }

        public List<string> GetFilesToRead()
        {
            List<string> retFiles = new List<string>();
            foreach(FileToRead file in files)
            {
                retFiles.Add(file.filename);
            }
            return retFiles;
        }
        public void GetDataFromFiles()
        {
            foreach (FileToRead file in files)
            {
                file.Evaluate();
            }
        }
        public List<BinaryData> GetBinaryDataToEvaulate()
        {
            return BinaryDataToEvaulate;
        }
        public void AddFile(string file)
        {
            if (System.IO.File.Exists(file))
            {
                if (file.Split('.').Length < 2)
                {
                    Logger.log($@"File:{file} Does not have an extension, this is an error", true);
                }
                Logger.log($@"Adding file {file} to list of files to Evaulate");
                files.Add(AddDataInternal(file));
            }
            else
            {
                Logger.log($@"File:{file} Does not exist, not adding", true);
            }
        }
        public void AddType(string Name, Action<string> Function, bool OverWrite = false)
        {
            if (!OverWrite && FileTypesExtensions.ContainsKey(Name.ToUpper()))
            {
                Logger.log($@"{Name} is already defined as an Extension", true);
            }
            else
            {
                FileTypesExtensions[Name.ToUpper()] = Function;
            }
        }
        private FileToRead AddDataInternal(FileToRead file)
        {
            Logger.log($@"Looking up file extension for file {file}");
            string extension = file.filename.Split('.').Last();
            if (FileTypesExtensions.ContainsKey(extension.ToUpper()))
            {
                Logger.log($@"Found file extension and defined evaulation function for {file}");
                file.setEvalFunction(FileTypesExtensions[extension.ToUpper()]);
                return file;
            }
            else
            {
                throw new Exception($@"extension for:{file.filename.ToUpper()} is not known");
            }
        }
        void DefineTypes()
        {
            Logger.log($"Defining file exstensions that can be evaluated");
            //for simplicity I will define a few types
            AddType("XML", (filename) =>
            {
                Logger.log($@"Parsing XML file {filename}");
                XElement Root = XElement.Load(filename);
                IEnumerable<XElement> Data = Root.Elements();

                foreach (var data in Data)
                {
                    double value1;
                    double value2;
                    string description;
                    bool error = false;

                    error |= !double.TryParse(data.Elements().First(x => x.Name.ToString().ToUpper() == "Value1".ToUpper()).Value, out value1);
                    //I decided that "twenty" is not valid data for this test, as it was not covered in the document
                    //if you would want to allow it as actual text, there are decently sized converters and code documentation online
                    //however that appeared to be outside of the scope of the test
                    error |= !double.TryParse(data.Elements().First(x => x.Name.ToString().ToUpper() == "Value2".ToUpper()).Value, out value2);
                    description = data.Descendants().First(x => x.Name.ToString().ToUpper() == "Description".ToUpper()).Value;
                    //normally I would try to split the errors up into which occurence happened, but for simplicity, all errors throw same 
                    if (error)
                    {
                        Logger.log($"Parsing Error occurred in\n{data.ToString()}\nOne or more Values could not be parsed as a double\n", true); //this would be a log error
                        continue; //we don't add errored data
                    }
                    BinaryData BData = new BinaryData(value1,
                                                      value2,
                                                      data.Name.ToString(),
                                                      description);
                    BinaryDataToEvaulate.Add(BData);
                }
            });
            AddType("JSON", (filename) =>
            {
                //this is left blank intentionally for the reader to fill in how they want the file to be read
            });
        }
        public List<BinaryData> getDataToEvaluate()
        {
            return BinaryDataToEvaulate;
        }
    }
    public class Operator
    {
        public string OperatorName;
        public Func<double, double, double> function;
        public Operator(string name, Func<double, double, double> f)
        {
            function = f;
            OperatorName = name;
        }
    }
    public class MathCombinator
    {
        Dictionary<string, Operator> BinaryOperations = new Dictionary<string, Operator>();
        List<BinaryData> CurrentData = new List<BinaryData>();

        public MathCombinator()
        {
            DefineTypes();
        }
        public MathCombinator(List<BinaryData> data)
        {
            DefineTypes();
            foreach (BinaryData Data in data)
            {
                AddDataInternal(Data);
            }
            CurrentData = data;
        }
        public void AddBinaryData(BinaryData data)
        {
            CurrentData.Add(data);
        }
        private void AddDataInternal(BinaryData Data)
        {
            Logger.log($"Assigning functions for evaulation and Operators for BinaryData {Data.Description}");
            if (BinaryOperations.ContainsKey(Data.ActionName.ToUpper()))
            {
                Logger.log($"Found Action to assign to BinaryData");
                Operator o = BinaryOperations[Data.ActionName.ToUpper()];
                Data.OperatorName = o.OperatorName;
                Data.setEvalFunction(o.function);
            }
            else
            {
                Logger.log($@"Action:{Data.ActionName.ToUpper()} is not known, skipping", true);
            }
        }

        public void AddData(BinaryData data)
        {
            AddDataInternal(data);
        }
        public void EvaluateData()
        {
            foreach (BinaryData data in CurrentData)
            {
                data.Evaluate();
            }
        }
        public void printData()
        {
            foreach (BinaryData data in CurrentData)
            {
                Console.WriteLine(data.Print()); //not logger, as the user specifically asked to print it
            }
        }
        void DefineTypes()
        {
            Logger.log($"Defining types of Actions that can be done");
            //for simplicity I will define a few types
            AddType("Add", "+", (f1, f2) => { return f1 + f2; });
            AddType("Multiply", "*", (f1, f2) => { return f1 * f2; });
            AddType("Subtract", "-", (f1, f2) => { return f1 - f2; });
            AddType("Divide", "/", (f1, f2) => { return f1 / f2; });
            AddType("Power", "^", (f1, f2) => { return Math.Pow(f1, f2); });
            AddType("Power", "^", (f1, f2) => { return Math.Pow(f1, f2); }); // leaving this here as a pseudo unit test
        }
        //technically we could go farther and allow the func to take any three evaulateable data types, but that seems overkill
        public void AddType(string Name, string OperatorChar, Func<double, double, double> Function, bool OverWrite = false)
        {
            if (!OverWrite && BinaryOperations.ContainsKey(Name.ToUpper()))
            {
                Logger.log($@"{Name} is already defined");
            }
            else
            {
                Operator o = new Operator(OperatorChar, Function);
                BinaryOperations[Name.ToUpper()] = o;
            }
        }

    }
    public class BinaryData
    {
        public string Description;
        double Value1;
        double Value2;
        public string ActionName;
        public string OperatorName;

        Func<double, double, double> Returnvalue;
        double EvaluatedData;
        bool evaluated = false;
        public void setEvalFunction(Func<double, double, double> func)
        {
            Returnvalue = func;
        }
        public BinaryData(double v1, double v2, string actionName, string description)
        {
            Value1 = v1;
            Value2 = v2;
            ActionName = actionName;
            Description = description;
        }
        public double Evaluate()
        {
            Logger.log($"Evaluating data {Description}| {ActionName} |({Value1}{OperatorName}{Value2})");
            evaluated = true;
            return EvaluatedData = Returnvalue.Invoke(Value1, Value2);
        }

        public string Print()
        {
            if (evaluated)
            {
                //it would be trivial to place some .splits to remove the "TURN10" that shows up in the descriptor; I will leave that for the reader
                return $@"{Description.Replace(";", " - ")}| {ActionName} |({Value1}{OperatorName}{Value2}={EvaluatedData})";
            }
            else
            {
                throw new Exception($@"Attempted to print Unevaluated Data {Description}|{Value1}|{ActionName}|{Value2}");
            }
        }
    }


}
