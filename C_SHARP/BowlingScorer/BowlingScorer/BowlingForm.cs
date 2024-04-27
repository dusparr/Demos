using BowlingScorer;
using System;
using System.Net.NetworkInformation;
using System.Windows.Forms;

public class BowlingForm : Form
{
    private TextBox[,] scoreTextBoxes = new TextBox[10, 3];
    private Label[] scoreLabels = new Label[10];
    private Button btnReset;
    private TextBox txtTotalScore;
    private BowlingGame bowlingGame = new BowlingGame();
    private bool suppressTextChange = false;

    public BowlingForm()
    {
        InitializeComponents();
    }

    private void InitializeComponents()
    {
        this.Width = 800;
        this.Height = 300;
        this.Text = "Bowling Score Tracker";

        // Total score display textbox
        txtTotalScore = new TextBox();
        txtTotalScore.Location = new System.Drawing.Point(350, 150);
        txtTotalScore.Size = new System.Drawing.Size(100, 20);
        txtTotalScore.ReadOnly = true; // Make this textbox readonly
        this.Controls.Add(txtTotalScore);

        for (int i = 0; i < 10; i++)
        {
            for (int j = 0; j < (i == 9 ? 3 : 2); j++) // 3 boxes for the 10th frame
            {
                scoreTextBoxes[i, j] = new TextBox();
                scoreTextBoxes[i, j].Location = new System.Drawing.Point(50 + i * 70, 50 + j * 30);
                scoreTextBoxes[i, j].Size = new System.Drawing.Size(50, 20);
                scoreTextBoxes[i, j].Tag = i; // Storing frame index in Tag
                scoreTextBoxes[i, j].TextChanged += new EventHandler(this.ScoreTextChanged);
                this.Controls.Add(scoreTextBoxes[i, j]);
            }

            scoreLabels[i] = new Label();
            scoreLabels[i].Location = new System.Drawing.Point(50 + i * 70, 120);
            scoreLabels[i].Size = new System.Drawing.Size(50, 20);
            scoreLabels[i].Text = "Frame " + (i + 1);
            this.Controls.Add(scoreLabels[i]);
        }

        btnReset = new Button();
        btnReset.Text = "Reset";
        btnReset.Location = new System.Drawing.Point(350, 180);
        btnReset.Click += new EventHandler(ResetGame);
        this.Controls.Add(btnReset);
    }

    private void ScoreTextChanged(object sender, EventArgs e)
    {
        if (suppressTextChange) return;

        TextBox currentBox = (TextBox)sender;
        int frameIndex = (int)currentBox.Tag;
        int textBoxIndex = FindTextBoxIndex(frameIndex, currentBox);


        try
        {
            string rollInput = currentBox.Text.ToUpper().Trim();
            if (rollInput == "_")
            {
                // Skip processing for "_" input and do not move to next input
                return;
            }
            int pins = ParseRoll(rollInput);
            if (pins < 0 || !IsValidInput(rollInput, frameIndex, textBoxIndex))
            {
                RevertInvalidInput(frameIndex, textBoxIndex);
                return;  // Prevent roll index increment and process further
            }
            if(textBoxIndex != 0) 
            { 
                if(frameIndex < 9)
                {
                    int first = ParseRoll(scoreTextBoxes[frameIndex, textBoxIndex].Text);
                    if(first + pins == 10) //this is a user error spare
                    {
                        currentBox.Text = "/";
                        return;
                    }
                }
                if (frameIndex == 9)
                {
                    int first = ParseRoll(scoreTextBoxes[frameIndex, textBoxIndex].Text);
                    if (textBoxIndex == 1)
                    {
                        if (first + pins == 10) //this is a user error spare
                        {
                            currentBox.Text = "/";
                            return;
                        }
                    }
                    else if (textBoxIndex == 2)
                    {
                        
                        if (scoreTextBoxes[frameIndex, textBoxIndex].Text.ToUpper() != "X" || scoreTextBoxes[frameIndex, textBoxIndex].Text != "/") //this is a user error spare
                        {
                            currentBox.Text = "";
                            return;
                        }
                    }
                }
            }
            //bowlingGame.Roll(pins);
            SetRolls();
            UpdateScores();
            HandleNextInput(frameIndex, textBoxIndex, rollInput);
        }
        catch (Exception ex)
        {
            MessageBox.Show(ex.Message);
            RevertInvalidInput(frameIndex, textBoxIndex);
        }
    }

    private bool IsValidInput(string input, int frameIndex, int textBoxIndex)
    {
        // Add specific rules here, for example:
        // Prevent entering any number or "/" after "X" in the same frame (1-9)
        if (frameIndex < 9 && input.ToUpper() == "X" && textBoxIndex == 0)
        {
            return true; // Valid strike input
        }
        if (frameIndex < 9 && textBoxIndex == 1 && scoreTextBoxes[frameIndex, 0].Text.ToUpper() == "X")
        {
            return input == "_"; // Only "_" is valid after a strike in non-10th frames
        }
        if ((frameIndex < 9 && input == "/" && textBoxIndex != 1) || (frameIndex == 9 && input == "/" && textBoxIndex == 0))
        {
            return false;
        }
        if ((frameIndex == 9 && input == "/" && textBoxIndex == 2) && (frameIndex == 9 && input == "/" && scoreTextBoxes[frameIndex, 1].Text.ToUpper() == "/"))
        {
            return false;
        }
        return true;
    }
    private void HandleNextInput(int frameIndex, int textBoxIndex, string rollInput)
    {
        if (frameIndex < 9 && rollInput.ToUpper() == "X")  // If it's a strike in frames 1-9
        {
            suppressTextChange = true;
            scoreTextBoxes[frameIndex, 1].Text = "_";  // Automatically fill in "_" for the second roll
            suppressTextChange = false;
            if (frameIndex < 9)  // Move focus to the first box of the next frame
            {
                scoreTextBoxes[frameIndex + 1, 0].Focus();
            }
        }
        else if (textBoxIndex == 0 && frameIndex < 9)  // Normal progression after first roll if not a strike
        {
            scoreTextBoxes[frameIndex, 1].Focus();
        }
        else if (frameIndex < 9)  // Move to the next frame's first roll if it's the second roll
        {
            scoreTextBoxes[frameIndex + 1, 0].Focus();
        }
        else if (frameIndex == 9)  // Special handling in the 10th frame
        {
            if (textBoxIndex < 2) // Move within the 10th frame as long as it's not the last roll
            {
                scoreTextBoxes[frameIndex, textBoxIndex + 1].Focus();
            }
        }
    }
    private void RevertInvalidInput(int frameIndex, int textBoxIndex)
    {
        suppressTextChange = true;
        if (textBoxIndex == 1 && scoreTextBoxes[frameIndex, 0].Text.ToUpper() == "X")
        {
            scoreTextBoxes[frameIndex, textBoxIndex].Text = "_"; // Force correction for second roll after a strike
        }
        else
        {
            scoreTextBoxes[frameIndex, textBoxIndex].Text = ""; // Clear incorrect input
        }
        suppressTextChange = false;
    }

    private int ParseRoll(string input)
    {
        if (input.ToUpper() == "X") return 10;
        if (input == "/")  // Handle spare within ScoreTextChanged, not here
        {
            return 0;// throw new InvalidOperationException("Spare input should be calculated contextually.");
        }
        if (int.TryParse(input, out int pins))
        {
            return pins;  // Numeric input
        }
        if (input == "_")
        {
            return 0;  // Treat '_' as 0 pins for the purpose of advancing the roll index
        }
        if (input == string.Empty)
        {
            return 0;
        }
        throw new InvalidOperationException("Invalid input. Please enter only numbers, 'X' for strikes, or '/' for spares.");
    }


    private int FindTextBoxIndex(int frameIndex, TextBox currentBox)
    {
        for (int j = 0; j < (frameIndex == 9 ? 3 : 2); j++)
        {
            if (scoreTextBoxes[frameIndex, j] == currentBox)
                return j;
        }
        return -1; // Not found, should not happen
    }

    private void UpdateScores()
    {
        int score = bowlingGame.Score();
        txtTotalScore.Text = bowlingGame.Score().ToString();
        for (int i = 0; i < 10; i++)
        {
            scoreLabels[i].Text = $"Frame {i + 1}: {score}";
        }
    }

    private void ResetGame(object sender, EventArgs e)
    {
        suppressTextChange = true;
        for (int i = 0; i < 10; i++)
        {
            for (int j = 0; j < (i == 9 ? 3 : 2); j++)
            {
                scoreTextBoxes[i, j].Text = "";
            }
            scoreLabels[i].Text = "Frame " + (i + 1);
        }
        txtTotalScore.Text = "";  // Clear the total score display
        bowlingGame = new BowlingGame();
        suppressTextChange = false;
    }

    private void SetRolls()
    {
        int[] rolls = new int[21];
        int rollindex = 0;
        for (int i = 0; i < 10; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                if (j == 2 && i != 9) { continue; }
                string StrVal = scoreTextBoxes[i, j].Text;
                if (StrVal.ToUpper() == "X")
                {
                    rolls[rollindex] = 10;
                    rollindex++;
                    if (i < 9)
                        break;
                }
                else if (StrVal.ToUpper() == "/")
                {
                    rolls[rollindex] = 10-rolls[rollindex-1];
                    rollindex++;
                }
                else if (StrVal == string.Empty)
                {
                    rolls[rollindex] = 0;
                    rollindex++;
                }
                else
                {
                    rolls[rollindex] = ParseRoll(StrVal);
                    rollindex++;
                }
            }
        }
        bowlingGame.setRolls(rolls);
    }

}
