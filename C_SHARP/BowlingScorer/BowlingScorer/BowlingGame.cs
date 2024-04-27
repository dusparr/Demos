using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace BowlingScorer
{
    public class BowlingGame
    {
        private int[] rolls = new int[21];  // Maximum number of rolls possible
        //using 11 for spare

        private int currentRoll = 0;

        public void Roll(int pins)
        {
            if (currentRoll >= rolls.Length)
                throw new InvalidOperationException("Exceeded the allowable number of rolls.");

            rolls[currentRoll] = pins;
            currentRoll++;  // Increment only after a valid roll
        }

        public void setRolls(int[] NewRolls) 
        {
            for (int i = 0; i<21; i++)
                rolls[i] = NewRolls[i];
        }
        
        public int Score()
        {
            int score = 0;
            int index = 0;  // Index for rolls array
            for (int frame = 0; frame < 10; frame++)
            {
                if (IsStrike(index))  // Check if it's a strike
                {
                    score += 10 + StrikeBonus(index);
                    index++;  // Skip only one roll index because the strike takes up the whole frame
                }
                else if (IsSpare(index))  // Check if it's a spare
                {
                    if (index+2 < 21 && rolls[index + 1] == 12)
                    {
                        score += 10; //handling the really odd case of 0,/ 0,/ 
                    }
                    else
                    {
                        score += 10 + rolls[index + 2];  // Add spare bonus
                    }
                    if(index+2 < 21 && frame == 9)
                    {
                        score += rolls[index + 2];
                    }
                    index +=2;  // Move index past the two rolls that make up the spare
                }
                else
                {
                    score += rolls[index] + rolls[index + 1];  // Normal frame score
                    index += 2;  // Move index past the two rolls
                    if (index < 21 && frame == 9)
                    {
                        score += rolls[index];
                    }
                }
            }
            return score;
        }

        private bool IsStrike(int index)
        {
            // A strike is always followed by skipping the next roll in frames 1-9
            return rolls[index] == 10;  // Only check strike logic for the first roll of each frame or any roll in the 10th frame
        }

        private bool IsSpare(int index)
        {
            return rolls[index+1] + rolls[index] == 10;
        }

        private int StrikeBonus(int index)
        {
            // Ensure index is within the bounds and calculate strike bonus
            int bonus = 0;
            if (index + 1 < rolls.Length) bonus += rolls[index + 1];
            if (index + 2 < rolls.Length) bonus += rolls[index + 2];
            return bonus;
        }
    }

}
