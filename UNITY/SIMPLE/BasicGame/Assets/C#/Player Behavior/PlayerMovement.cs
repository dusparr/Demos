using UnityEngine;
using UnityEngine.UI;
using System.Collections;


public class PlayerMovement : MonoBehaviour {

    public float rotSpeed = 10.0f;
    public float speed = 10.0f;
    private Rigidbody RB = null;
    public enum RotationAxis { MouseX, MouseY, MouseXandY };
    public RotationAxis AxisToChangeOn;
    public Text ScoreText;
    // Use this for initialization
    public int score = 0;
    public float SensitivityHor;
    public float SensitivityVer;
    public float Ymin = -45.0f;
    public float Ymax = 45.0f;
    public GameObject cam;

    private float _rotX =0.0f;
    private float _rotY = 0.0f;
    private bool Jumpable = true;
    void Start () {
        RB = GetComponent<Rigidbody>();
        setScore();

    }

    // Update is called once per frame
    void FixedUpdate()
    {
        //int Up = Input.GetKey(KeyCode.UpArrow)  ? 1 : 0;
        //int Down = Input.GetKey(KeyCode.DownArrow) ? 1 : 0;
        //int Left = Input.GetKey(KeyCode.LeftArrow) ? 1 : 0;
        //int Right = Input.GetKey(KeyCode.RightArrow) ? 1 : 0;


        //if (Input.GetKey(KeyCode.LeftArrow))
        //transform.Rotate(new Vector3(-rotSpeed, 0, 0),Space.Self);//, rotSpeed, Space.Self);
        //else if (Input.GetKey(KeyCode.RightArrow))
        //transform.Rotate(new Vector3(rotSpeed, 0, 0), Space.Self);// rotSpeed, Space.Self);
        //Vector3 Force = new Vector3(/*(-Left + Right) * speed*/0, 0, (Up + -Down) * speed);
        if (Input.GetKey(KeyCode.UpArrow))
            RB.AddForce(RB.transform.forward * speed, ForceMode.Impulse);
        else if (Input.GetKey(KeyCode.DownArrow))
            RB.AddForce(-RB.transform.forward * speed, ForceMode.Impulse);
        if (Input.GetKey(KeyCode.Space) && Jumpable == true)
        {
            print("jump?");
            RB.AddForce(RB.transform.up * speed*10, ForceMode.Impulse);
            Jumpable = false;
        }
        MouseCheck();
    }
    public GameObject mouseMaker;
    void MouseCheck()
    {
        
        switch (AxisToChangeOn)
        {
            case RotationAxis.MouseX:
            case RotationAxis.MouseXandY:
                {
                    transform.localEulerAngles = HorizantalRotate();
                    //print("MOuse Axis X : " + Input.GetAxis("Mouse X"));
                }
                if (AxisToChangeOn == RotationAxis.MouseXandY) goto case RotationAxis.MouseY; // in C++ I would set a bool in mouseXandY and fall through if true
                break;
            case RotationAxis.MouseY:
                {
                    cam.transform.localEulerAngles = VerticalRotate();
                    //print(_rotX);
                }
                break;
        }
        if (Input.GetMouseButtonDown(0))
        {
            print("MAYBE 1?");
            var mousePos = Input.mousePosition;
            mousePos.z = 2.0f;       // we want 2m away from the camera position
            //print();
            //var objectPos = Camera.current.ScreenToWorldPoint(mousePos);
            //print(mouseMaker.ToString());
            //Instantiate(mouseMaker, objectPos, Quaternion.identity);
            //print("MAYBE 2?");
        }
    }

    private Vector3 VerticalRotate()
    {
        _rotX -= Input.GetAxis("Mouse Y") * SensitivityVer;
        
        cam.transform.Rotate(_rotX, 0, 0, Space.World);
        _rotX = Mathf.Clamp(_rotX, Ymin, Ymax);
        return new Vector3(_rotX, 0, 0.0f);
    }

    private Vector3 HorizantalRotate()
    {
        _rotY += Input.GetAxis("Mouse X") * SensitivityHor;
        transform.Rotate(0, _rotY, 0, Space.World);
        return new Vector3(0, _rotY, 0.0f);
    }

    void OnTriggerEnter(Collider other)
    {
        switch (other.gameObject.tag)
        {
            case "Money":
                {
                    other.gameObject.SetActive(false);
                    print("What");
                    score += 1;
                    setScore();
                }
                break;
            case "Ground":
                {
                    print("jump = true;");
                    Jumpable = true;
                }
                break;
        }
    }
    
    void setScore()
    {
        ScoreText.text = "Count: " + score;
    }
    
}
