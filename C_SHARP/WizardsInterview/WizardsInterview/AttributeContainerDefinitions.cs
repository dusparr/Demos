using System.Collections.Generic;
using System.Linq;
using System;



namespace CardData
{
    namespace Base
    {
        public abstract class BaseAttributeContainer<Attribute, EffectOperation, ModType> : ILayeredAttributes<Attribute, EffectOperation, ModType>
            where Attribute : System.Enum where EffectOperation : System.Enum
        {
            protected abstract class LayerDictionary
            {
                public delegate ModType def();
                public static def DEFAULT = null;

                //change this to use just the Dictionary<int, List<LayeredEffectDefinition<Attribute, EffectOperation, ModType>>> effect
                protected Dictionary<Attribute, Dictionary<int, ModType>> LayerTotals = new Dictionary<Attribute, Dictionary<int, ModType>>();
                protected Dictionary<int, List<LayeredEffectDefinition<Attribute, EffectOperation, ModType>>> Dict = new Dictionary<int, List<LayeredEffectDefinition<Attribute, EffectOperation, ModType>>>();
                protected Dictionary<string, Tuple<Attribute, int>> effectByAttrLayerPos = new Dictionary<string, Tuple<Attribute, int>>();


                public System.Collections.Generic.List<LayeredEffectDefinition<Attribute, EffectOperation, ModType>> this[int layer]
                {
                    get { if (Dict.ContainsKey(layer)) return Dict[layer]; else return null; }
                    //set => SetValue(key, value);
                }
                /// <summary>
                /// Theser return or set the underlying Totals Structure, not the effects on the data
                /// </summary>
                /// <param name="a"></param>
                /// <param name="layer"></param>
                /// <returns></returns>

                public abstract ModType this[Attribute a, int layer]
                {
                    set;
                    
                    get;
                    
                }
                public List<int> keys
                {
                    get => Dict.Keys.ToList();
                }




                public bool remove(string EffectName)
                {
                    if (effectByAttrLayerPos.ContainsKey(EffectName))
                    {
                        int index = Dict[effectByAttrLayerPos[EffectName].Item2].FindIndex((x) => x.Name == EffectName);
                        if (index >= 0)
                        {
                            Dict[effectByAttrLayerPos[EffectName].Item2].RemoveAt(index);
                            effectByAttrLayerPos.Remove(EffectName);
                            return true;
                        }
                        return false;
                    }
                    return false;
                }
                public void assignLayer(LayeredEffectDefinition<Attribute, EffectOperation, ModType> layerEffect)
                {
                    if (!Dict.ContainsKey(layerEffect.Layer))
                    {
                        Dict[layerEffect.Layer] = new List<LayeredEffectDefinition<Attribute, EffectOperation, ModType>>();
                    }
                    Dict[layerEffect.Layer].Add(layerEffect);

                    effectByAttrLayerPos[layerEffect.Name] = new Tuple<Attribute, int>(layerEffect.Attribute, layerEffect.Layer);
                }


                
                
                public int Count
                {
                    get => Dict.Count();
                }
                public void Clear()
                {
                    LayerTotals.Clear();
                    Dict.Clear();
                }

            }
            protected LayerDictionary Dict;
            protected Dictionary<Attribute, ModType> baseAttributes = new Dictionary<Attribute, ModType>();
            protected Dictionary<Attribute, ModType> RealAttributes = new Dictionary<Attribute, ModType>();
            protected Attribute LookupForInheritedEnumType;
            protected void Construct(Dictionary<Attribute, ModType> defaults)
            {
                LayerDictionary.DEFAULT = GetDefaultValueForEffect;
                SetToDefaults();
                if (defaults != null)
                {
                    baseAttributes = defaults;
                    RealAttributes = defaults;
                    //this allows you to give some or all of the values for defaults, if you want to define them externally
                    //I left (in the comments) the way that doesn't involve checking on every type for invalid; but it is more fragile to code changes
                    foreach (Attribute Akey in System.Enum.GetValues(LookupForInheritedEnumType.GetType()))
                    //for (int Akey = (int)AttributeKey.Invalid + 1; Akey <= (int)AttributeKey.Controller; Akey++) //+1 to invalid to ignore invalid
                    {
                        if (defaults.ContainsKey((Attribute)Akey))
                        {
                            baseAttributes[(Attribute)Akey] = defaults[(Attribute)Akey];
                            RealAttributes[(Attribute)Akey] = defaults[(Attribute)Akey];
                        }
                    }
                }
            }
            protected bool genericAddLayeredEffect(LayeredEffectDefinition<Attribute, EffectOperation, ModType> effect, bool Quick =false)
            {
                if (System.Enum.IsDefined(effect.Attribute.GetType(), effect.Attribute) && System.Enum.IsDefined(effect.Operation.GetType(), effect.Operation) && System.Enum.IsDefined(effect.Conditional.GetType(),effect.Conditional))
                {
                    Dict.assignLayer(effect);
                    ModType retVal;
                    bool canApply = ApplyEffectToNum(Dict[effect.Attribute, effect.Layer], effect, out retVal);
                    Dict[effect.Attribute, effect.Layer] = retVal;

                    ModType NULL = GetDefaultValueForEffect();
                    if(!Quick) SetCurrentAttributes();
                    return true;
                }
                else
                {
                    Logger.log("PASSED IN AN INVALID OPERATION OR ATTRIBUTE");
                    return false;
                    //throw new System.Exception("This seems to be invalid by default");
                    //I can't really see a reason to handle invalid? with the possible exception of not wanting to unwind the stack
                }
            }

            protected void SetCurrentAttributes(int skipPastLayer = int.MinValue)
            {
                var layers = Dict.keys;
                layers.Sort();
                RealAttributes = new Dictionary<Attribute, ModType>(baseAttributes);
                foreach (int layer in layers)
                {
                    foreach (var effect in Dict[layer])
                    {
                        ModType attri = RealAttributes[effect.Attribute];
                        bool isValid = ApplyEffectToNum(attri, effect, out attri);
                        if(isValid)
                        {
                            RealAttributes[effect.Attribute] = attri;
                        }
                    }
                }
            }

            

            protected ModType applyLayer(Attribute key, List<LayeredEffectDefinition<Attribute, EffectOperation, ModType>> layer, ModType currAttri)
            {
                foreach (var effect in layer)
                {
                    bool isValid = ApplyEffectToNum(currAttri, effect, out currAttri);
                    if (!isValid)
                    {
                        //something has gone terribly wrong;
                        Logger.log("something has gone terribly wrong;");
                    }
                }
                return currAttri;
            }

            protected void SetToDefaults(bool toBase = false)
            {
                //I left (in the comments) the way that doesn't involve checking on every type for invalid; but it is more fragile to code changes
                foreach (Attribute Akey in System.Enum.GetValues(LookupForInheritedEnumType.GetType()))
                //for (int Akey = (int)AttributeKey.Invalid + 1; Akey <= (int)AttributeKey.Controller; Akey++) //+1 to invalid to ignore invalid
                {

                    baseAttributes[(Attribute)Akey] = toBase ? baseAttributes[(Attribute)Akey] : GetDefaultValueForEffect();
                    RealAttributes[(Attribute)Akey] = toBase ? baseAttributes[(Attribute)Akey] : GetDefaultValueForEffect();
                }
                Dict.Clear();
            }

            public abstract bool GenericGetCurrentAttribute(Attribute key, out ModType value);
            
            public void GenericClearLayeredEffects()
            {
                SetToDefaults(true);
            }

            public bool GenericSetBaseAttribute(Attribute key, ModType value)
            {
                if (System.Enum.IsDefined(LookupForInheritedEnumType.GetType(), key))
                {
                    baseAttributes[key] = value;
                    //then make changes
                    SetCurrentAttributes();
                    return true;
                }

                return false;
            }
            //for testing Purposes
            public int getCountOfEffectDictionary()
            {
                return Dict.Count;
            }
            public virtual bool AddLayeredEffect(LayeredEffectDefinition<Attribute, EffectOperation, ModType> effect)
            {
                return genericAddLayeredEffect(effect);
            }
            public virtual void ClearLayeredEffects()
            {
                GenericClearLayeredEffects();
            }
            public virtual bool GetCurrentAttribute(Attribute key, out ModType value)
            {
                value = GetDefaultValueForEffect();
                return GenericGetCurrentAttribute(key, out value);

            }
            public virtual bool SetBaseAttribute(Attribute key, ModType value)
            {
                return GenericSetBaseAttribute(key, value);
            }

            public bool RemoveEffect(LayeredEffectDefinition<Attribute, EffectOperation, ModType> effect)
            {

                if (Dict.remove(effect.Name))
                {
                    SetCurrentAttributes();
                }
                return false;
            }

            protected abstract bool ApplyEffectToNum(ModType Attribute, LayeredEffectDefinition<Attribute, EffectOperation, ModType> effect, out ModType ret);
            protected abstract ModType GetDefaultValueForEffect();
        }
    }

    //The below two type containers allow encapsulation of both struct and class types
    public abstract class ClassAttributeContainer<Attribute, EffectOperation, ModType> : Base.BaseAttributeContainer<Attribute, EffectOperation, ModType>
        where Attribute : System.Enum where EffectOperation : System.Enum where ModType : class
    {
        protected new class LayerDictionary : Base.BaseAttributeContainer<Attribute, EffectOperation, ModType>.LayerDictionary
        {
            public override ModType this[Attribute a, int layer]
            {
                set
                {

                    if (value != null)
                    {
                        LayerTotals[a][layer] = (ModType)value;
                    }

                }
                get
                { 
                    if(!LayerTotals.ContainsKey(a))
                    {
                        LayerTotals.Add(a, new Dictionary<int, ModType>());
                    }
                    var ALayers = LayerTotals[a];
                    if (ALayers != null)
                    {
                        if (!ALayers.ContainsKey(layer))
                            return null;
                        return (ModType)ALayers[layer];
                    }
                    return null;
                }
            }
        }
        public ClassAttributeContainer() 
        {
            Dict = new LayerDictionary();
            Construct(null);
        }
        public ClassAttributeContainer(Dictionary<Attribute, ModType> defaults) 
        {
            Dict = new LayerDictionary();
            Construct(defaults);
        }
        public override bool GenericGetCurrentAttribute(Attribute key, out ModType value)
        {
            SetCurrentAttributes();
            if (System.Enum.IsDefined(LookupForInheritedEnumType.GetType(), key))
            {
                value = RealAttributes[key];
                return true;
            }
            Logger.log("ATTEMPTED TO GET AN INVALID ATTRIBUTE");
            value = null;
            return false;
            //leaving this here for now, even though I hoisted the log out, typically use an out int and a bool return instead
            //but I am not changing the implementation 
            //*public bool GetCurrentAttribute(AttributeKey key, out int ret)
            //is the approach I would use

        }
        //almost purely abstract
    }
    public abstract class StructAttributeContainer<Attribute, EffectOperation, ModType> : Base.BaseAttributeContainer<Attribute, EffectOperation, ModType>
        where Attribute : System.Enum where EffectOperation : System.Enum where ModType : struct
    {
        protected new class LayerDictionary : Base.BaseAttributeContainer<Attribute, EffectOperation, ModType>.LayerDictionary
        {
            public override ModType this[Attribute a, int layer]
            {
                set
                {
                    LayerTotals[a][layer] = (ModType)value;
                }
                get
                {
                    if (!LayerTotals.ContainsKey(a))
                    {
                        LayerTotals.Add(a, new Dictionary<int, ModType>());
                    }
                    var ALayers = LayerTotals[a];
                    if (ALayers != null)
                    {
                        if (!ALayers.ContainsKey(layer))
                            return DEFAULT();
                        return (ModType)ALayers[layer];
                    }
                    return DEFAULT();
                }
            }
        }
        public StructAttributeContainer() 
        {
            Dict = new LayerDictionary();
            Construct(null);
        }
        public StructAttributeContainer(Dictionary<Attribute, ModType> defaults) 
        {
            Dict = new LayerDictionary();
            Construct(defaults);
        }
        public override bool GenericGetCurrentAttribute(Attribute key, out ModType value)
        {
            SetCurrentAttributes();
            if (System.Enum.IsDefined(LookupForInheritedEnumType.GetType(), key))
            {
                value = RealAttributes[key];
                return true;
            }
            Logger.log("ATTEMPTED TO GET AN INVALID ATTRIBUTE");
            value = GetDefaultValueForEffect();
            return false;
            //leaving this here for now, even though I hoisted the log out, typically use an out int and a bool return instead
            //but I am not changing the implementation 
            //*public bool GetCurrentAttribute(AttributeKey key, out int ret)
            //is the approach I would use

        }
        //almost purely abstract
    }

    // VERSION DETAILS: C#InterviewPreScreen v1.1 2016-05-17
}