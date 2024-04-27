namespace CardData
{
    namespace Operations
    {
        public enum StrOperations
        {
            Change = 0,
            Append
        }
        public enum IntOperations
        {
            //Invalid = 0,

            /// <summary>
            /// Set a value, discarding any prior value.
            /// </summary>
            Set = 0,

            /// <summary>
            /// Add to the prior value.
            /// </summary>
            Add,

            /// <summary>
            /// Subtract from the prior value.
            /// </summary>
            Subtract,

            /// <summary>
            /// Multiply the prior value by the layered effect's Modification.
            /// </summary>
            Multiply,

            /// <summary>
            /// Perform a bitwise "or" operation.
            /// </summary>
            BitwiseOr,

            /// <summary>
            /// Perform a bitwise "and" operation.
            /// </summary>
            BitwiseAnd,

            /// <summary>
            /// Perform a bitwise "exclusive or" operation.
            /// </summary>
            BitwiseXor,

            Divide,

            Power,
        }
    }
}
// VERSION DETAILS: C#InterviewPreScreen v1.1 2016-05-17