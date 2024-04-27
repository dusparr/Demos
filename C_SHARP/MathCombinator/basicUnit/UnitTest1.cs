using Microsoft.VisualStudio.TestTools.UnitTesting;
using MathCombinator;
using System;
using System.Collections.Generic;
using System.Reflection;




namespace basicUnit
{
    [TestClass]
    public class UnitTest1
    {
        List<string> filesToReadReal = new List<string> { @"math.xml" };
        List<string> filesToReadFake = new List<string> { @"math.xml", "NONEXISTANT.xml" };

        List<string> filesToReadForBinaryTest = new List<string> { @"UnitTest.xml" };
        List<BinaryData> ExpectedData = new List<BinaryData>
        {
            new BinaryData(40,2,"Add", "Joe; SUM;TURN10"),
            new BinaryData(2,3, "Multiply", "Sam;MULTIPLY;TURN10"),
        };

        //public BinaryData(double v1, double v2, string actionName, string description)


        [TestMethod]
        public void TestMethod1()
        {
            Reader ReaderReal = new Reader(filesToReadReal);
            List<string> filesFromReader = ReaderReal.GetFilesToRead();
            FilesCheck(filesFromReader, filesToReadReal);


            Reader ReaderBadData = new Reader(filesToReadFake);
            filesFromReader = ReaderBadData.GetFilesToRead();
            FilesCheck(filesFromReader, filesToReadReal);

            Reader ReaderForBinary = new Reader(filesToReadForBinaryTest);
            ReaderForBinary.GetDataFromFiles();
            List<BinaryData> dataForUnitTest = ReaderForBinary.GetBinaryDataToEvaulate();

            BinaryDataCheck(dataForUnitTest, ExpectedData);

            MathCombinator.MathCombinator mathCombinator = new MathCombinator.MathCombinator(ReaderReal.getDataToEvaluate());
            mathCombinator.EvaluateData();
            mathCombinator.printData();
        }

        private void BinaryDataCheck(List<BinaryData> dataForUnitTest, List<BinaryData> dataToCheckAgainst)
        {
            Assert.AreEqual(dataToCheckAgainst.Count, dataForUnitTest.Count);
            //not sure of order so will do this via "contains for each"
            foreach (BinaryData BD in dataToCheckAgainst)
            {
                Assert.AreEqual(true, dataForUnitTest.Contains(BD));
            }
        }

        private void FilesCheck(List<string> filesFromReader, List<string> filesToCheckAgainst)
        {
            Assert.AreEqual(filesToCheckAgainst.Count, filesFromReader.Count);
            for (int i = 0; i < filesToCheckAgainst.Count; i++)
            {
                Assert.AreEqual(filesToCheckAgainst[i], filesFromReader[i]);
            }
        }
    }
}

