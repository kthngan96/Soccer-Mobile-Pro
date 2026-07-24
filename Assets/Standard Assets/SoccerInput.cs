using System.Collections.Generic;
using UnityEngine;
using UnityEngine.InputSystem;
using UnityEngine.InputSystem.EnhancedTouch;

using InputSystemTouch = UnityEngine.InputSystem.EnhancedTouch.Touch;
using InputSystemTouchPhase = UnityEngine.InputSystem.TouchPhase;

public enum SoccerTouchPhase
{
    Began,
    Moved,
    Stationary,
    Ended,
    Canceled
}

public struct SoccerTouch
{
    public int fingerId;
    public Vector2 position;
    public SoccerTouchPhase phase;
    public int tapCount;
}

public static class SoccerInput
{
    private static readonly List<SoccerTouch> touchBuffer = new List<SoccerTouch>(8);
    private static int touchBufferFrame = -1;
    private static bool initialized;

    [RuntimeInitializeOnLoadMethod(RuntimeInitializeLoadType.BeforeSceneLoad)]
    private static void InitializeOnLoad()
    {
        EnsureInitialized();
    }

    public static bool PointerPressedThisFrame
    {
        get
        {
            EnsureInitialized();
            Mouse mouse = Mouse.current;
            return mouse != null && mouse.leftButton.wasPressedThisFrame;
        }
    }

    public static bool PointerReleasedThisFrame
    {
        get
        {
            EnsureInitialized();
            Mouse mouse = Mouse.current;
            return mouse != null && mouse.leftButton.wasReleasedThisFrame;
        }
    }

    public static Vector2 PointerPosition
    {
        get
        {
            EnsureInitialized();
            Mouse mouse = Mouse.current;
            return mouse == null ? Vector2.zero : mouse.position.ReadValue();
        }
    }

    public static int TouchCount
    {
        get
        {
            RefreshTouches();
            return touchBuffer.Count;
        }
    }

    public static List<SoccerTouch> Touches
    {
        get
        {
            RefreshTouches();
            return touchBuffer;
        }
    }

    public static float HorizontalAxis
    {
        get { return Move.x; }
    }

    public static float VerticalAxis
    {
        get { return Move.y; }
    }

    public static Vector2 Move
    {
        get
        {
            EnsureInitialized();
            Vector2 move = Vector2.zero;
            Keyboard keyboard = Keyboard.current;
            if (keyboard != null)
            {
                if (keyboard.leftArrowKey.isPressed || keyboard.aKey.isPressed)
                {
                    move.x -= 1f;
                }

                if (keyboard.rightArrowKey.isPressed || keyboard.dKey.isPressed)
                {
                    move.x += 1f;
                }

                if (keyboard.downArrowKey.isPressed || keyboard.sKey.isPressed)
                {
                    move.y -= 1f;
                }

                if (keyboard.upArrowKey.isPressed || keyboard.wKey.isPressed)
                {
                    move.y += 1f;
                }
            }

            Gamepad gamepad = Gamepad.current;
            if (gamepad != null)
            {
                Vector2 gamepadMove = gamepad.leftStick.ReadValue();
                if (gamepadMove.sqrMagnitude > move.sqrMagnitude)
                {
                    move = gamepadMove;
                }
            }

            return Vector2.ClampMagnitude(move, 1f);
        }
    }

    public static bool PrimaryActionPressed
    {
        get
        {
            EnsureInitialized();
            Keyboard keyboard = Keyboard.current;
            Gamepad gamepad = Gamepad.current;
            return (keyboard != null && keyboard.spaceKey.isPressed) ||
                   (gamepad != null && gamepad.buttonSouth.isPressed);
        }
    }

    public static bool PausePressedThisFrame
    {
        get
        {
            EnsureInitialized();
            Keyboard keyboard = Keyboard.current;
            Gamepad gamepad = Gamepad.current;
            return (keyboard != null && keyboard.escapeKey.wasPressedThisFrame) ||
                   (gamepad != null && gamepad.startButton.wasPressedThisFrame);
        }
    }

    public static bool ScreenshotPressedThisFrame
    {
        get
        {
            EnsureInitialized();
            Keyboard keyboard = Keyboard.current;
            return keyboard != null && keyboard.kKey.wasPressedThisFrame;
        }
    }

    public static bool WarmupAPressedThisFrame
    {
        get
        {
            EnsureInitialized();
            Keyboard keyboard = Keyboard.current;
            return keyboard != null && keyboard.aKey.wasPressedThisFrame;
        }
    }

    public static bool WarmupBPressedThisFrame
    {
        get
        {
            EnsureInitialized();
            Keyboard keyboard = Keyboard.current;
            return keyboard != null && keyboard.bKey.wasPressedThisFrame;
        }
    }

    public static bool WarmupCPressedThisFrame
    {
        get
        {
            EnsureInitialized();
            Keyboard keyboard = Keyboard.current;
            return keyboard != null && keyboard.cKey.wasPressedThisFrame;
        }
    }

    public static SoccerTouch GetTouch(int index)
    {
        RefreshTouches();
        return touchBuffer[index];
    }

    private static void RefreshTouches()
    {
        EnsureInitialized();
        if (touchBufferFrame == Time.frameCount)
        {
            return;
        }

        touchBufferFrame = Time.frameCount;
        touchBuffer.Clear();

        foreach (InputSystemTouch touch in InputSystemTouch.activeTouches)
        {
            SoccerTouch soccerTouch = new SoccerTouch
            {
                fingerId = touch.finger.index,
                position = touch.screenPosition,
                phase = ConvertPhase(touch.phase),
                tapCount = touch.tapCount
            };
            touchBuffer.Add(soccerTouch);
        }
    }

    private static SoccerTouchPhase ConvertPhase(InputSystemTouchPhase phase)
    {
        switch (phase)
        {
            case InputSystemTouchPhase.Began:
                return SoccerTouchPhase.Began;
            case InputSystemTouchPhase.Moved:
                return SoccerTouchPhase.Moved;
            case InputSystemTouchPhase.Ended:
                return SoccerTouchPhase.Ended;
            case InputSystemTouchPhase.Canceled:
                return SoccerTouchPhase.Canceled;
            default:
                return SoccerTouchPhase.Stationary;
        }
    }

    private static void EnsureInitialized()
    {
        if (initialized)
        {
            return;
        }

        EnhancedTouchSupport.Enable();
        initialized = true;
    }
}
