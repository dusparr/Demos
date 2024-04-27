using System;
using IntCardLayeredEffect = CardData.LayeredEffectDefinition<CardData.Attributes.IntAttributes, CardData.Operations.IntOperations, int>;
namespace CardData
{
    /// <summary>
    /// This object defines the current attributes of a card, sans rules, flavortext and misc identifiers
    /// it does this by having containing a dictionary of base and current attributes
    /// and a complicated dictionary of the current effects on the card
    /// note that it doesn't impliment any way to remove individual layers, only adding or removing all
    /// </summary>
    public class IntCardAttributesContainer : StructAttributeContainer<Attributes.IntAttributes, Operations.IntOperations, int>
    {


        //now the only definition is where we need to affect both the attribute and the operation
        protected override bool ApplyEffectToNum(int Attribute, IntCardLayeredEffect effect, out int ret)
        {
            
            if (ConditionalMet(ref effect))
            {
                switch (effect.Operation)
                {
                    case Operations.IntOperations.Set:
                        Attribute = effect.Modification;
                        break;
                    case Operations.IntOperations.Add:
                        Attribute += effect.Modification;
                        break;
                    case Operations.IntOperations.Subtract:
                        Attribute -= effect.Modification;
                        break;
                    case Operations.IntOperations.Multiply:
                        Attribute *= effect.Modification;
                        break;
                    case Operations.IntOperations.BitwiseOr:
                        Attribute |= effect.Modification;
                        break;
                    case Operations.IntOperations.BitwiseAnd:
                        Attribute &= effect.Modification;
                        break;
                    case Operations.IntOperations.BitwiseXor:
                        Attribute ^= effect.Modification;
                        break;
                    case Operations.IntOperations.Divide:
                        Attribute /= effect.Modification;
                        break;
                    case Operations.IntOperations.Power:
                        Attribute = Convert.ToInt32(Math.Pow((double)Attribute, (double)effect.Modification));
                        break;
                    //case CardEffectOperation.Invalid: //fallthrough do to this not supposed to happen and why are we even sending this?
                    default:
                        //ehh
                        Logger.log("attempting to apply an invalid operation");
                        //again this should be 
                        ret = GetDefaultValueForEffect();
                        return false;
                        //throw new System.Exception("I don't know what you want"); //as above, this is just wanting to not handle invalid
                        break;
                }

                ret = Attribute;
                return true;
            }
            ret = GetDefaultValueForEffect();
            return false;
        }

        private bool ConditionalMet(ref IntCardLayeredEffect effect)
        {
            bool apply = false;
            if (effect.Conditional == Attributes.IntAttributes.Invalid)
            {
                apply = true;
            }
            else
            {
                if (RealAttributes[effect.Conditional] == effect.ConditionalValue)
                {
                    apply = true;
                }
                else
                {
                    apply = false;
                }
            }

            return apply;
        }

        protected override int GetDefaultValueForEffect()
        {
            return 0;
        }
        
    }
}
// VERSION DETAILS: C#InterviewPreScreen v1.1 2016-05-17