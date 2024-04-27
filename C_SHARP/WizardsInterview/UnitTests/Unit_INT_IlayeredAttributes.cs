using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Collections.Generic;
using System.IO;
#region Name Masking for extremely long names
//masking Lambdas and Vistor function ptrs for the purposes of more readable code
using AllAttributesLambda = System.Func<CardData.Attributes.IntAttributes, int, CardData.IntCardAttributesContainer, int, CardData.IntCardAttributesContainer>; //man, I am so used to seeing this in C++, but it feels weird to see it in C#
using Visitor = System.Collections.Generic.List<System.Func<CardData.Attributes.IntAttributes, int, CardData.IntCardAttributesContainer, int, CardData.IntCardAttributesContainer>>;
using IntCardLayer = CardData.LayeredEffectDefinition<CardData.Attributes.IntAttributes, CardData.Operations.IntOperations, int   >;
#endregion
#region ILayeredAttributeSpecific
using CardData;
using CardData.Operations;
using CardData.Attributes;
#endregion

namespace Unit_IlayeredAttributes
{
    [TestClass]
    public class UnitTest1
    {

        #region Logger
        public class InternalLogger
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
        #endregion
        #region lambdas to call
        /// All of these functions are lambdas that wrap around basic functionality of AttributeContainer
        /// they all go by a simple set of rules
        /// if you don't pass a AttributeContainer to them; they make one up, setup defaults (via constructor)
        /// then call the "operation" on the AttributeContainer
        /// then if we made a new AttributeContainer (you passed null for the container) it does a simple assert
        /// it can only do it when making a new one (or in some simple cases like set)
        /// due to the fact that it doesn't know what other layers have been set/are effecting the attribute
        /// in other cases
        /// it then returns the AttributeContainer
        /// 


        static int Layer = 0;
        #region special Case lambdas
        /// <summary>
        /// Set an Attribute at a layer to a specific number
        /// </summary>
        static AllAttributesLambda setBase = (key, value, Acontainer, layer) => {
            bool check = false;
            if (Acontainer == null)
            {
                Acontainer = new IntCardAttributesContainer();
                check = true;
            }
            Assert.AreEqual(true,Acontainer.SetBaseAttribute(key, value));
            if (check)
            {
                int retval;
                Assert.AreEqual(true, Acontainer.GetCurrentAttribute(key, out retval));
                Assert.AreEqual(value, retval);
            }
            return Acontainer;
        };
        /*
        /// <summary>
        /// this is a special case; I don't want you to be able to set an invalid anywhere
        /// so we specifically attempt to set it and expect it to throw
        /// </summary>
        static Action<int> checkInvalid = ( value) => {
            CardAttributesContainer a = new CardAttributesContainer();
            Assert.AreEqual(false, a.SetBaseAttribute(CardAttributeKey.Invalid, value));
            int retval;
            Assert.AreEqual(false, a.GetCurrentAttribute(CardAttributeKey.Invalid, out retval));
            Assert.AreEqual(-1, retval);
        };
        */
        /// <summary>
        /// Calls ClearLayeredEffects on the AttributeContainer
        /// </summary>
        static AllAttributesLambda Clear = (key, value, Acontainer, layer) => {
            bool check = false;
            if (Acontainer == null)
            {
                Acontainer = new IntCardAttributesContainer();
                check = true;
            }
            Acontainer.ClearLayeredEffects();
            if (check)
            {
                int retval;
                Assert.AreEqual(true, Acontainer.GetCurrentAttribute(key, out retval));
                Assert.AreEqual(value, retval);
            }
            return Acontainer;
        };
        #endregion


        static AllAttributesLambda Add = (key, value, Acontainer, layer) => {
            return CallOperation(key, value, ref Acontainer, layer, IntOperations.Add,        new Action<int, int>((baseVal, realVal) => Assert.AreEqual(baseVal + value, realVal)));
        };                                                                                      
        static AllAttributesLambda BitwiseOr = (key, value, Acontainer, layer) => {                 
            return CallOperation(key, value, ref Acontainer, layer, IntOperations.BitwiseOr,  new Action<int, int>((baseVal, realVal) => Assert.AreEqual(baseVal | value, realVal)));
        };
        static AllAttributesLambda BitwiseAnd = (key, value, Acontainer, layer) => {
            return CallOperation(key, value, ref Acontainer, layer, IntOperations.BitwiseAnd, new Action<int, int>((baseVal,realVal) => Assert.AreEqual (baseVal & value, realVal)));
        };
        static AllAttributesLambda Multiply = (key, value, Acontainer, layer) => {
            return CallOperation(key, value, ref Acontainer, layer, IntOperations.Multiply,   new Action<int, int>((baseVal, realVal) => Assert.AreEqual(baseVal * value, realVal)));
        };                                                                                      
        static AllAttributesLambda Subtract = (key, value, Acontainer, layer) => {               
            return CallOperation(key, value, ref Acontainer, layer, IntOperations.Subtract,   new Action<int, int>((baseVal, realVal) => Assert.AreEqual(baseVal - value, realVal)));
        };                                                                                     
        static AllAttributesLambda Set = (key, value, Acontainer, layer) => {               
            return CallOperation(key, value, ref Acontainer, layer, IntOperations.Set,        new Action<int, int>((baseVal, realVal) => Assert.AreEqual(value,           realVal)));
        };
        static AllAttributesLambda bitwiseXor = (key, value, Acontainer, layer) => {
            return CallOperation(key, value, ref Acontainer, layer, IntOperations.BitwiseXor, new Action<int, int>((baseVal, realVal) => Assert.AreEqual(baseVal ^ value, realVal)));
        };
        static AllAttributesLambda addCond = (key, value, Acontainer, layer) => {
            return CallOperation(key, value, ref Acontainer, layer, IntOperations.Add, new Action<int, int>((baseVal, realVal) => Assert.AreEqual(baseVal + value, realVal)), IntAttributes.Color, 7);
        };
        //if we were to have a EffectOperator.SetToZero it would look like so
        /*
          static AttributeLambda SetToZero = (key, value, Acontainer, layer) => {
            return CallOperation(key, value, ref Acontainer, layer, EffectOperation.SetToZero, new Action<int, int>((baseVal, realVal) => Assert.AreEqual(0, realVal)));
          };
        */
        /// <summary>
        /// This is used to Verify after calling a "Worker = " type Lambda call
        /// it basically wraps around an assert against a specfic value and the AttributeContainer.GetCurrentAttribute
        /// </summary>
        static AllAttributesLambda CheckAgainstValue = (key, value, Acontainer, layer) => {
            int retval;
            Assert.AreEqual(true, Acontainer.GetCurrentAttribute(key, out retval));
            InternalLogger.log($@"Worker: {key}=>{retval}");
            //this is used for layer checking mostly
            Assert.AreEqual(value, retval);
            return Acontainer;
        };

        static AllAttributesLambda RemoveLayerMinusN = (key, value, Acontainer, layer) => {
            
            if(Acontainer != null)
            {
                IntCardLayer rem = CreateLayeredEffect(key, value, layer, IntOperations.Set, false);
                rem.Name = (Layer - value - 1).ToString();
                Acontainer.RemoveEffect(rem);
            }
            return Acontainer;
        };


        #endregion

        #region Helper Functions

        /// <summary>
        /// This is the true Helper function that allows all of the lambas to be fairly simple
        /// </summary>
        /// <param name="key">Which AttributeKey to run on</param>
        /// <param name="value">A Value to pass to the CreateLayeredEffect function</param>
        /// <param name="Acontainer">if null, create a new one with defaults, otherwise modify the passed in AttributesContainer</param>
        /// <param name="layer">What Layer you want to run this on</param>
        /// <param name="operation">What operation to call</param>
        /// <param name="assert">A Lambda to call as your assert, For example; new Action<int, int>((baseVal, realVal) => Assert.AreEqual(baseVal + value, realVal)) </param>
        /// <returns>either a new Attributes container (if passed in as null) or the passed in Attributes container (with modifications from operation)</returns>
        private static IntCardAttributesContainer CallOperation(IntAttributes key, int value, ref IntCardAttributesContainer Acontainer, int layer, IntOperations operation, Action<int, int> assert, IntAttributes cond = IntAttributes.Invalid, int condvalue = 0)
        {
            bool check = false; //all of these checks are not valid if there are layers involved as I cannot do the math ahead of time
            if (Acontainer == null)
            {
                Acontainer = new IntCardAttributesContainer();
                check = true;
            }

            int baseWorkerValue;
            Assert.AreEqual(true, Acontainer.GetCurrentAttribute(key, out baseWorkerValue));
            IntCardLayer add = CreateLayeredEffect(key, value, layer, operation, true, cond, condvalue);
            Assert.AreEqual(true, Acontainer.AddLayeredEffect(add));
            if (check)
            {
                int retval;
                Assert.AreEqual(true, Acontainer.GetCurrentAttribute(key, out retval));
                Assert.AreEqual(value, retval);
            }   
            return Acontainer;
        }


        /// <summary>
        /// Creates a layered effect and returns it
        /// </summary>
        /// <param name="key">Which AttributeKey to run on</param>
        /// <param name="value">A Value to pass to the CreateLayeredEffect function</param>
        /// <param name="layer">What Layer you want to run this on</param>
        /// <param name="operation">What operation to call</param>
        /// <returns>the new layered effect</returns>
        private static IntCardLayer CreateLayeredEffect(IntAttributes key, int value, int layer, IntOperations operation, bool setname = true, IntAttributes conditional = IntAttributes.Invalid, int CondValue = 0)
        {
            IntCardLayer mult = new IntCardLayer();
            mult.Attribute = key;
            mult.Layer = layer;
            mult.Modification = value;
            mult.Operation = operation;
            mult.Conditional = conditional;
            mult.ConditionalValue = CondValue;
            if(setname) mult.Name += Layer++;
            return mult;
        }

        /// <summary>
        /// creates a new Attributes container, runs your list of lambdas on it, passing value 
        /// asserts if any fail
        /// </summary>
        /// <param name="funcs">list of lambdas to call</param>
        /// <param name="value">value to pass to lambdas</param>
        private static void VisitAllAttributesSimple(Visitor funcs, int value)
        {
            IntCardAttributesContainer worker = new IntCardAttributesContainer();
            worker = Visit(funcs, value, worker, 0);
        }
        /// <summary>
        /// creates a new Attributes container or takes an existing one, runs your list of lambdas on it, passing value 
        /// asserts if any fail
        /// </summary>
        /// <param name="funcs">list of lambdas to call</param>
        /// <param name="value">value to pass to lambdas</param>
        /// <param name="worker">if null, creates new container, otherwise modifies the passed in one</param>
        /// <returns>the modified AttributeContainer</returns>
        private static IntCardAttributesContainer VisitAllAttributesPassWorker(Visitor funcs, int value, IntCardAttributesContainer worker)
        {
            worker = Visit(funcs, value, worker, 0);
            return worker;
        }
        /// <summary>
        /// creates a new Attributes container or takes an existing one, runs your list of lambdas on it, passing value
        /// asserts if any fail
        /// </summary>
        /// <param name="funcs">list of lambdas to call</param>
        /// <param name="value">value to pass to lambdas</param>
        /// <param name="worker">if null, creates new container, otherwise modifies the passed in one</param>
        /// <param name="layer">the layer to run the lambdas on</param>
        /// <returns>the modified AttributeContainer</returns>
        private static IntCardAttributesContainer VisitAllAttributesPassWorkerWithLayer(Visitor funcs, int value, IntCardAttributesContainer worker, int layer)
        {
            worker = Visit(funcs, value, worker, layer);
            return worker;
        }

        private static IntCardAttributesContainer VisitAnAttributePassingWorkerWithLayer(Visitor funcs, int value, IntCardAttributesContainer worker, IntAttributes a, int layer)
        {
            worker = VisitAttribute(funcs, value, worker, layer, a);
            return worker;
        }

        /// <summary>
        /// visits the list of functions on the Attributes container, at the layer passed
        /// </summary>
        /// <param name="funcs">list of lambdas to call</param>
        /// <param name="value">value to pass to lambdas</param>
        /// <param name="worker">if null, creates new container, otherwise modifies the passed in one</param>
        /// <param name="layer">the layer to run the lambdas on</param>
        /// <returns>the modified AttributeContainer</returns>
        private static IntCardAttributesContainer Visit(Visitor funcs, int value, IntCardAttributesContainer worker, int layer)
        {
            foreach (IntAttributes Akey in System.Enum.GetValues(typeof(IntAttributes)))
            //for (int Akey = (int)AttributeKey.Invalid + 1; Akey <= (int)AttributeKey.Controller; Akey++) //+1 to invalid to ignore invalid
            {
                //if (Akey != CardAttributeKey.Invalid)
                //{
                    foreach (var func in funcs)
                    {
                        worker = func((IntAttributes)Akey, value, worker, layer);
                    }
                //}
            }

            return worker;
        }

        private static IntCardAttributesContainer VisitAttribute(Visitor funcs, int value, IntCardAttributesContainer worker, int layer, IntAttributes a)
        {

            foreach (var func in funcs)
            {
                worker = func((IntAttributes)a, value, worker, layer);
            }

            return worker;
        }

        #endregion
        [TestMethod]
        public void IGNORE() { } //this is the first test case ; for some reason a 5-9ms "bootup time" is added to the first test case so I Isolate it here instead of in Setbase


        #region TestMethods
        [TestMethod]
        public void SetBaseAttributeTests()
        {
            VisitAllAttributesSimple(new Visitor { setBase }, 1);

            VisitAllAttributesSimple(new Visitor { setBase }, -1);

            VisitAllAttributesSimple(new Visitor { setBase }, 0898098); //randomish
            
        }
        /*
        [TestMethod]
        public void SetInvalidTest()
        {
            checkInvalid(-1);
        }
        */

        [TestMethod]
        public void AddAttributeTests()
        {
            VisitAllAttributesSimple(new Visitor
            {
                setBase,
                Add
            },
            1);

            VisitAllAttributesSimple(new Visitor
            {
                setBase,
                Add,
                Add,
                Add,
            },
            2);

            IntCardAttributesContainer worker = null;

            worker = VisitAllAttributesPassWorker(new Visitor { setBase           }, 1, worker);
            //allValuesShouldBe1
            worker = VisitAllAttributesPassWorker(new Visitor { CheckAgainstValue }, 1, worker);
            worker = VisitAllAttributesPassWorker(new Visitor { Add               }, 1, worker);
            worker = VisitAllAttributesPassWorker(new Visitor { CheckAgainstValue }, 2, worker);
            worker = VisitAllAttributesPassWorker(new Visitor { setBase           }, 2, worker);
            //since we change the base from 1=>2 then it should go from (B)1+(L0)1 = 2 to (B)2+(L0)1 = 3
            worker = VisitAllAttributesPassWorker(new Visitor { CheckAgainstValue }, 3, worker); 

        }

        [TestMethod]
        public void SubAttributeTests()
        {
            IntCardAttributesContainer worker = new IntCardAttributesContainer();

            worker = VisitAllAttributesPassWorker(new Visitor { setBase           }, 1, worker);
            //allValuesShouldBe1
            worker = VisitAllAttributesPassWorker(new Visitor { CheckAgainstValue }, 1, worker);
            worker = VisitAllAttributesPassWorker(new Visitor { Subtract          }, 1, worker);
            worker = VisitAllAttributesPassWorker(new Visitor { CheckAgainstValue }, 0, worker);
        }

        [TestMethod]
        public void RemovalTests()
        {

            
            IntCardAttributesContainer worker = new IntCardAttributesContainer();
            worker = VisitAnAttributePassingWorkerWithLayer(new Visitor { setBase }, 3, worker, IntAttributes.Power, -1);
            worker = VisitAnAttributePassingWorkerWithLayer(new Visitor { Add }, 1, worker, IntAttributes.Power, -1);
            worker = VisitAnAttributePassingWorkerWithLayer(new Visitor { CheckAgainstValue }, 4, worker, IntAttributes.Power, -1);
            worker = VisitAnAttributePassingWorkerWithLayer(new Visitor { Multiply }, 2, worker, IntAttributes.Power, -1);
            worker = VisitAnAttributePassingWorkerWithLayer(new Visitor { CheckAgainstValue }, 8, worker, IntAttributes.Power, -1);
            //remove the +1, or the effect (N-1)
            worker = VisitAnAttributePassingWorkerWithLayer(new Visitor { RemoveLayerMinusN }, 1, worker, IntAttributes.Power, -1);
            //should be 6 now
            worker = VisitAnAttributePassingWorkerWithLayer(new Visitor { CheckAgainstValue }, 6, worker, IntAttributes.Power, -1);


        }
        [TestMethod]
        public void CondTest()
        {
            IntCardAttributesContainer worker = new IntCardAttributesContainer();
            worker = VisitAnAttributePassingWorkerWithLayer(new Visitor { setBase }, 6, worker, IntAttributes.Color, -1);
            worker = VisitAnAttributePassingWorkerWithLayer(new Visitor { setBase }, 1, worker, IntAttributes.Power, -1);
            worker = VisitAnAttributePassingWorkerWithLayer(new Visitor { addCond }, 6, worker, IntAttributes.Power, 1);
            worker = VisitAnAttributePassingWorkerWithLayer(new Visitor { CheckAgainstValue }, 1, worker, IntAttributes.Power, int.MaxValue);
            worker = VisitAnAttributePassingWorkerWithLayer(new Visitor { Add }, 1, worker, IntAttributes.Color, 2);
            worker = VisitAnAttributePassingWorkerWithLayer(new Visitor { addCond }, 6, worker, IntAttributes.Power, 3);
            worker = VisitAnAttributePassingWorkerWithLayer(new Visitor { CheckAgainstValue }, 7, worker, IntAttributes.Power, int.MaxValue);
        }


        [TestMethod]
        public void LayerTests()
        {
            //first seting all to 1
            int LAYER_M1 = -1;
            int LAYER1 = 1;
            int LAYER567 = 567;
            int baseVal = 1;

            IntCardAttributesContainer worker = new IntCardAttributesContainer();
            InternalLogger.log($@"Worker: SettingBase => {baseVal}");
            worker = VisitAllAttributesPassWorker(new Visitor { setBase           }, baseVal, worker    );
            //allValuesShouldBe1
            InternalLogger.log($@"Worker should hold all {baseVal}");
            worker = VisitAllAttributesPassWorker(new Visitor { CheckAgainstValue }, baseVal, worker    );
            InternalLogger.log($@"Worker holds all {baseVal}");                                         
            //then at layer 1 adding 1 -> should all be 2                                               
            InternalLogger.log($@"Worker: adding => {1} on Layer {LAYER1}");                            
            worker = VisitAllAttributesPassWorkerWithLayer(new Visitor { Add       }, 1, worker, LAYER1 );
            //allValuesShouldBe2
            InternalLogger.log($@"Worker should hold all {2}");
            worker = VisitAllAttributesPassWorker(new Visitor { CheckAgainstValue  }, 2, worker         );
            InternalLogger.log($@"Worker holds all {2}");
            //at layer 0 adding 1  ; base (1) + (layer0 (1)) + layer1 (1) = 3
            InternalLogger.log($@"Worker: adding => {1} on Layer {LAYER_M1}");
            worker = VisitAllAttributesPassWorkerWithLayer(new Visitor { Add       }, 1, worker, LAYER_M1);
            //allValuesShouldBe2
            InternalLogger.log($@"Worker should hold all {3}");
            worker = VisitAllAttributesPassWorker(new Visitor { CheckAgainstValue  }, 3, worker         );
            InternalLogger.log($@"Worker holds all {3}");
            //at layer 0 multiplying by 2 ; ((base (1) + layer0 (1)) * layer0(2)) + layer1 (1) = 5
            InternalLogger.log($@"Worker: Multiply => {2} on Layer {LAYER_M1}");
            worker = VisitAllAttributesPassWorkerWithLayer(new Visitor { Multiply }, 2, worker, LAYER_M1);
            //allValuesShouldBe 5
            InternalLogger.log($@"Worker should hold all {5}");
            worker = VisitAllAttributesPassWorker(new Visitor { CheckAgainstValue }, 5, worker          );
            InternalLogger.log($@"Worker holds all {5}");
            //at layer 567 multiplying by 2, 8 times ; ((base (1) + layer0 (1)) * layer0(2)) + layer1 (1) * 2^8 = 1280
            InternalLogger.log($@"Worker: Multiply => 2^8 on Layer {LAYER567}");
            worker = VisitAllAttributesPassWorkerWithLayer(new Visitor {
                Multiply, Multiply, Multiply, Multiply, Multiply, Multiply, Multiply, Multiply
                                                                                  }, 2, worker, LAYER567);
            //allValuesShouldBe 1280            
            InternalLogger.log($@"Worker should hold all {1280}");
            worker = VisitAllAttributesPassWorker(new Visitor { CheckAgainstValue }, 1280, worker);
            InternalLogger.log($@"Worker holds all {1280}");
            
            
            //finally just set the whole thing to 7 at layer intmax
            InternalLogger.log($@"Worker: SetValue => 7 on Layer {int.MaxValue}");
            worker = VisitAllAttributesPassWorkerWithLayer(new Visitor { Set }, 7, worker, int.MaxValue);
            //allValuesShouldBe7
            InternalLogger.log($@"Worker should hold all {7}");
            worker = VisitAllAttributesPassWorker(new Visitor { CheckAgainstValue }, 7, worker);
            InternalLogger.log($@"Worker holds all {7}");


            InternalLogger.log($@"Worker: BitwiseOr => 7|124 on Layer {int.MaxValue}");
            worker = VisitAllAttributesPassWorkerWithLayer(new Visitor {BitwiseOr }, 124, worker, int.MaxValue);
            //allValuesShouldBe7
            InternalLogger.log($@"Worker should hold all {7|124}");
            worker = VisitAllAttributesPassWorker(new Visitor { CheckAgainstValue }, 7|124, worker);
            InternalLogger.log($@"Worker holds all {7 | 124}");

            InternalLogger.log($@"Worker: BitwiseAnd => (7|124)&25 on Layer {int.MaxValue}");
            worker = VisitAllAttributesPassWorkerWithLayer(new Visitor {BitwiseAnd}, 25, worker, int.MaxValue);
            //allValuesShouldBe7
            InternalLogger.log($@"Worker should hold all {(7 | 124) & 25}");
            worker = VisitAllAttributesPassWorker(new Visitor { CheckAgainstValue }, (7 | 124) & 25, worker);
            InternalLogger.log($@"Worker holds all {(7 | 124) & 25}");

            InternalLogger.log($@"Worker: BitwiseXor => ((7|124)&25)^55 on Layer {int.MaxValue}");
            worker = VisitAllAttributesPassWorkerWithLayer(new Visitor {bitwiseXor}, 55, worker, int.MaxValue);
            //allValuesShouldBe7
            InternalLogger.log($@"Worker should hold all {((7 | 124) & 25)^55}");
            worker = VisitAllAttributesPassWorker(new Visitor { CheckAgainstValue }, ((7 | 124) & 25) ^ 55, worker);
            InternalLogger.log($@"Worker holds all {((7 | 124) & 25) ^ 55}");



            //now to clear everything
            InternalLogger.log($@"Worker: Clearing => 7 on Layer {int.MaxValue}");
            worker = VisitAllAttributesPassWorker(new Visitor { Clear             }, 2, worker        );
            InternalLogger.log($@"Worker should hold all {baseVal}");
            worker = VisitAllAttributesPassWorker(new Visitor { CheckAgainstValue }, baseVal, worker  );
            InternalLogger.log($@"Worker holds all {baseVal}");

            Assert.AreEqual(0, worker.getCountOfEffectDictionary()); //just to make sure all effects have been removed
            
        }
        #endregion



    }
}
