namespace CardData
{
    public class StrCardAttributesContainer : ClassAttributeContainer<Attributes.StrAttributes, Operations.StrOperations, string>
    {
        protected override bool ApplyEffectToNum(string Attribute, LayeredEffectDefinition<Attributes.StrAttributes, Operations.StrOperations, string> effect, out string ret)
        {
            switch (effect.Operation)
            {
                case Operations.StrOperations.Change:
                    Attribute = effect.Modification;
                    break;
                case Operations.StrOperations.Append:
                    Attribute += effect.Modification;
                    break;
                //case CardEffectOperation.Invalid: //fallthrough do to this not supposed to happen and why are we even sending this?
                default:
                    //ehh
                    Logger.log("ATTEMPTING TO APPLY AN EFFECT TO AN INVALID ATTRIBUTE");
                    //again this should be 
                    ret = null;
                    return false;
            }
            ret = Attribute;
            return true;
        }

        protected override string GetDefaultValueForEffect()
        {
            return "";
        }
    }
}
// VERSION DETAILS: C#InterviewPreScreen v1.1 2016-05-17