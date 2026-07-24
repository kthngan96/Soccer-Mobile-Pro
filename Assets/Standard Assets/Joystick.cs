using UnityEngine;

public class JoystickBoundary
{
    public Vector2 min = Vector2.zero;
    public Vector2 max = Vector2.zero;
}

public class Joystick : MonoBehaviour
{
    static private Joystick[] joysticks;
    static private bool enumeratedJoysticks = false;
    static private float tapTimeDelta = 0.1f;
    public bool touchPad;
    public float touchPadPressed = 0.15f;
    public float touchPadReleased = 0.025f;
    public Rect touchZone;
    public float maximumOffset = 0.5f;
    public Vector2 deadZone = Vector2.zero;
    public bool normalize = false;
    public bool roundVertical = false;
    public bool roundHorizontal = false;
    public float roundThreshold = 0.5f;
    public Vector2 position;
    public int tapCount;
    public Rect defaultRect;
    private int lastFingerId = -1;
    private float tapTimeWindow;
    private Vector2 fingerDownPos;
    private float fingerDownTime;
    private GUITexture gui;
    private JoystickBoundary guiBoundary = new JoystickBoundary();
    private Vector2 guiTouchOffset;
    private Vector2 guiCenter;
    private Vector3 tmpv3;
    private Rect tmprect;
    private Color tmpclr;

    float RoundPosition(float position)
    {
        if (position > 0 && position > roundThreshold)
            position = 1;
        else if (position < 0 && position < -roundThreshold)
            position = -1;
        else
            position = 0;

        return position;
    }

    public void Start()
    {
        gui = LegacyGuiUtility.GetOrAddGUITexture(gameObject);
        defaultRect = gui.pixelInset;

        if (touchPad)
        {
            if (gui.texture)
                touchZone = gui.pixelInset;
        }
        else
        {
            guiTouchOffset.x = defaultRect.width * maximumOffset;
            guiTouchOffset.y = defaultRect.height * maximumOffset;
            guiCenter.x = defaultRect.x + guiTouchOffset.x;
            guiCenter.y = defaultRect.y + guiTouchOffset.y;
            guiBoundary.min.x = defaultRect.x - guiTouchOffset.x;
            guiBoundary.max.x = defaultRect.x + guiTouchOffset.x;
            guiBoundary.min.y = defaultRect.y - guiTouchOffset.y;
            guiBoundary.max.y = defaultRect.y + guiTouchOffset.y;
        }
    }

    public void Disable()
    {
        gameObject.SetActive(false);
        enumeratedJoysticks = false;
    }

    public void ResetJoystick()
    {
        gui.pixelInset = defaultRect;
        lastFingerId = -1;
        position = Vector2.zero;
        fingerDownPos = Vector2.zero;

        if (touchPad)
        {
            tmpclr = gui.color;
            tmpclr.a = touchPadReleased;
            gui.color = tmpclr;
        }
    }

    public bool IsFingerDown()
    {
        return (lastFingerId != -1);
    }

    public void LatchedFinger(int fingerId)
    {
        if (lastFingerId == fingerId)
            ResetJoystick();
    }

    public void Update()
    {
        if (!enumeratedJoysticks)
        {
            joysticks = (Joystick[])FindObjectsOfType(typeof(Joystick));
            enumeratedJoysticks = true;
        }

        int count = SoccerInput.TouchCount;
        if (tapTimeWindow > 0)
            tapTimeWindow -= Time.deltaTime;
        else
            tapCount = 0;

        if (count == 0)
            ResetJoystick();
        else
        {
            for (int i = 0; i < count; i++)
            {
                SoccerTouch touch = SoccerInput.GetTouch(i);

                Vector2 guiTouchPos = touch.position - guiTouchOffset;

                bool shouldLatchFinger = false;
                if (touchPad)
                {
                    if (touchZone.Contains(touch.position))
                        shouldLatchFinger = true;
                }
                else if (gui.HitTest(touch.position))
                {
                    shouldLatchFinger = true;
                }

                // Latch the finger if this is a new touch
                if (shouldLatchFinger && (lastFingerId == -1 || lastFingerId != touch.fingerId))
                {

                    if (touchPad)
                    {
                        tmpclr = gui.color;
                        tmpclr.a = touchPadPressed;
                        gui.color = tmpclr;
                        lastFingerId = touch.fingerId;
                        fingerDownPos = touch.position;
                        fingerDownTime = Time.time;
                    }

                    lastFingerId = touch.fingerId;

                    // Accumulate taps if it is within the time window
                    if (tapTimeWindow > 0)
                        tapCount++;
                    else
                    {
                        tapCount = 1;
                        tapTimeWindow = tapTimeDelta;
                    }

                    // Tell other joysticks we've latched this finger
                    foreach (Joystick j in joysticks)
                    {
                        if (j != this)
                            j.LatchedFinger(touch.fingerId);
                    }
                }

                if (lastFingerId == touch.fingerId)
                {
                    if (touch.tapCount > tapCount)
                        tapCount = touch.tapCount;

                    if (touchPad)
                    {
                        position.x = Mathf.Clamp((touch.position.x - fingerDownPos.x) / (touchZone.width / 2), -1, 1);
                        position.y = Mathf.Clamp((touch.position.y - fingerDownPos.y) / (touchZone.height / 2), -1, 1);
                    }
                    else
                    {
                        tmprect = gui.pixelInset;
                        tmprect.x = Mathf.Clamp(guiTouchPos.x, guiBoundary.min.x, guiBoundary.max.x);
                        tmprect.y = Mathf.Clamp(guiTouchPos.y, guiBoundary.min.y, guiBoundary.max.y);
                        gui.pixelInset = tmprect;
                    }

                    if (touch.phase == SoccerTouchPhase.Ended || touch.phase == SoccerTouchPhase.Canceled)
                        ResetJoystick();
                }
            }
        }

        if (!touchPad)
        {
            position.x = (gui.pixelInset.x + guiTouchOffset.x - guiCenter.x) / guiTouchOffset.x;
            position.y = (gui.pixelInset.y + guiTouchOffset.y - guiCenter.y) / guiTouchOffset.y;
        }

        float absoluteX = Mathf.Abs(position.x);
        float absoluteY = Mathf.Abs(position.y);

        if (absoluteX < deadZone.x)
        {
            position.x = 0;
        }
        else if (normalize && roundHorizontal)
        {
            var posx = Mathf.Sign(position.x) * (absoluteX - deadZone.x) / (1 - deadZone.x);
            position.x = RoundPosition(posx);
        }
        else if (normalize)
        {
            position.x = Mathf.Sign(position.x) * (absoluteX - deadZone.x) / (1 - deadZone.x);
        }

        if (absoluteY < deadZone.y)
        {
            position.y = 0;
        }
        else if (normalize && roundVertical)
        {
            var posy = Mathf.Sign(position.y) * (absoluteY - deadZone.y) / (1 - deadZone.y);
            position.y = RoundPosition(posy);
        }
        else if (normalize)
        {
            position.y = Mathf.Sign(position.y) * (absoluteY - deadZone.y) / (1 - deadZone.y);
        }
    }
}
