namespace CardData
{
    // GOAL:
    //   Create a class that implements the following interface. Use your best
    //     judgement when it comes to design tradeoffs and implementation decisions.
    //   You may create any number of other classes to support your implementation.
    //   If you alter the given code, please maintain the intent of the original code
    //     and document why your alterations were necessary.


    /// <summary>
    /// Any object that implements this interface has a set of "base" attributes
    /// that represent the default state of that object. However, the game engine
    /// may apply one or more "layered effects" to modify those attributes. An
    /// object's "current" attributes are always equal to the base attributes
    /// with all layered effects applied, in the proper order. Any change to the
    /// base attribute or layered effects should immediately be reflected in the
    /// current attribute.
    /// </summary>
    interface ILayeredAttributes<Attribute, EffectOperation, ModType> where Attribute : System.Enum where EffectOperation : System.Enum
    {
        /// <summary>
        /// Set the base value for an attribute on this object. All base values
        /// default to 0 until set. Note that resetting a base attribute does not
        /// alter any existing layered effects.
        /// </summary>
        /// <param name="key">The attribute being set.</param>
        /// <param name="value">The new base value.</param>
        bool SetBaseAttribute(Attribute key, ModType value);

        /// <summary>
        /// Return the current value for an attribute on this object. Will
        /// be equal to the base value, modified by any applicable layered
        /// effects.
        /// </summary>
        /// <param name="key">The attribute being read.</param>
        /// <returns>The current value of the attribute, accounting for all layered effects.</returns>
        bool GetCurrentAttribute(Attribute key, out ModType value);

        /// <summary>
        /// Applies a new layered effect to this object's attributes. See
        /// LayeredEffectDefinition for details on how layered effects are
        /// applied. Note that any number of layered effects may be applied
        /// at any given time. Also note that layered effects are not necessarily
        /// applied in the same order they were added. (see LayeredEffectDefinition.Layer)
        /// </summary>
        /// <param name="effect">The new layered effect to apply.</param>
        bool AddLayeredEffect(LayeredEffectDefinition<Attribute, EffectOperation, ModType> effect);

        /// <summary>
        /// Removes all layered effects from this object. After this call,
        /// all current attributes will be equal to the base attributes.
        /// </summary>
        void ClearLayeredEffects();

        bool RemoveEffect(LayeredEffectDefinition<Attribute, EffectOperation, ModType> effect);
    }

    /// <summary>
    /// Parameter struct for AddLayeredEffect(...)
    /// </summary>
    public struct LayeredEffectDefinition<Attributes, EffectsOperation, ModType> where Attributes : System.Enum where EffectsOperation : System.Enum
    {
        /// <summary>
        /// Which attribute this layered effect applies to.
        /// </summary>
        public Attributes Attribute { get; set; }

        /// <summary>
        /// What mathematical or bitwise operation this layer performs.
        /// See EffectOperation for details.
        /// </summary>
        public EffectsOperation Operation { get; set; }

        /// <summary>
        /// The operand used for this layered effect's Operation.
        /// For example, if Operation is EffectOperation.Add, this is the
        /// amount that is added.
        /// </summary>
        public ModType Modification { get; set; }

        /// <summary>
        /// Which layer to apply this effect in. Smaller numbered layers
        /// get applied first. Layered effects with the same layer get applied
        /// in the order that they were added. (timestamp order)
        /// </summary>
        public int Layer { get; set; }

        public string Name { get; set; }

        public Attributes Conditional { get; set; }
        public ModType ConditionalValue { get; set; }

    }
}
// VERSION DETAILS: C#InterviewPreScreen v1.1 2016-05-17