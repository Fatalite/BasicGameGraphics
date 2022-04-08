#include "Camera/Camera.h"

namespace library
{
    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Camera::Camera
      Summary:  Constructor
      Modifies: [m_yaw, m_pitch, m_moveLeftRight, m_moveBackForward,
                 m_moveUpDown, m_travelSpeed, m_rotationSpeed,
                 m_padding, m_cameraForward, m_cameraRight, m_cameraUp,
                 m_eye, m_at, m_up, m_rotation, m_view].
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Camera::Camera definition (remove the comment)
    --------------------------------------------------------------------*/
    Camera::Camera(_In_ const XMVECTOR& position)
        :m_travelSpeed(0.01),m_rotationSpeed(0.001),
        m_moveUpDown(0),m_moveBackForward(0),m_moveLeftRight(0),
        m_yaw(0),m_pitch(0),
        m_padding(NULL),
        m_cameraForward(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f)),
        m_cameraRight(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f)),
        m_cameraUp(DEFAULT_UP),
        m_eye(position),m_at(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)),m_up(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)),
        m_rotation(XMMatrixIdentity()),m_view(XMMatrixIdentity())
    {};
    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Camera::GetEye
      Summary:  Returns the eye vector
      Returns:  const XMVECTOR&
                  The eye vector
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Camera::GetEye definition (remove the comment)
    --------------------------------------------------------------------*/

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Camera::GetAt
      Summary:  Returns the at vector
      Returns:  const XMVECTOR&
                  The at vector
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Camera::GetAt definition (remove the comment)
    --------------------------------------------------------------------*/

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Camera::GetUp
      Summary:  Returns the up vector
      Returns:  const XMVECTOR&
                  The up vector
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Camera::GetUp definition (remove the comment)
    --------------------------------------------------------------------*/

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Camera::GetView
      Summary:  Returns the view matrix
      Returns:  const XMMATRIX&
                  The view matrix
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Camera::GetView definition (remove the comment)
    --------------------------------------------------------------------*/

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Camera::HandleInput
      Summary:  Sets the camera state according to the given input
      Args:     const DirectionsInput& directions
                  Keyboard directional input
                const MouseRelativeMovement& mouseRelativeMovement
                  Mouse relative movement input
                FLOAT deltaTime
                  Time difference of a frame
      Modifies: [m_yaw, m_pitch, m_moveLeftRight, m_moveBackForward,
                 m_moveUpDown].
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Camera::HandleInput definition (remove the comment)
    --------------------------------------------------------------------*/


    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Camera::Update
      Summary:  Updates the camera based on its state
      Args:     FLOAT deltaTime
                  Time difference of a frame
      Modifies: [m_rotation, m_at, m_cameraRight, m_cameraUp,
                 m_cameraForward, m_eye, m_moveLeftRight,
                 m_moveBackForward, m_moveUpDown, m_up, m_view].
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Camera::Update definition (remove the comment)
    --------------------------------------------------------------------*/
    const XMVECTOR& Camera::GetEye() const {
        return m_eye;
    };
    const XMVECTOR& Camera::GetAt() const {
        return m_at;
    };
    const XMVECTOR& Camera::GetUp() const {
        return m_up;
    };
    const XMMATRIX& Camera::GetView() const {
        return m_view;
    };

    void Camera::HandleInput(_In_ const DirectionsInput& directions, _In_ const MouseRelativeMovement& mouseRelativeMovement, _In_ FLOAT deltaTime) {
        //FLOAT m_yaw;
       // FLOAT m_pitch;

        //FLOAT m_moveLeftRight;
        //FLOAT m_moveBackForward;
        //FLOAT m_moveUpDown;
        // 
        //mouse movement를 라디언으로 변환
        //float dx = XMConvertToRadians();
        //float dy = XMConvertToRadians();
        
        if (m_pitch + mouseRelativeMovement.Y * m_rotationSpeed * deltaTime
            < XM_PIDIV2 
            && 
            m_pitch + mouseRelativeMovement.Y * m_rotationSpeed * deltaTime
            > -XM_PIDIV2) {
            m_pitch = m_pitch + mouseRelativeMovement.Y * m_rotationSpeed * deltaTime;
        }

        m_yaw = m_yaw + mouseRelativeMovement.X * m_rotationSpeed * deltaTime ;

        if (directions.bUp == TRUE) {
            m_moveUpDown = 1.0f;
        }
        else if(directions.bDown == TRUE){
            m_moveUpDown = -1.0f;

        }

        if (directions.bFront == TRUE) {
            m_moveBackForward = 1.0f;
        }
        else if (directions.bBack == TRUE) {
            m_moveBackForward = -1.0f;

        }
        if (directions.bLeft == TRUE) {
            m_moveLeftRight = -1.0f;
        }
        else if (directions.bRight == TRUE) {
            m_moveLeftRight = +1.0f;

        }
        Update(deltaTime);

    };
    void Camera::Update(_In_ FLOAT deltaTime) {
            
            //두 축으로 로테이션 매트릭스를 만든다.
            m_rotation = XMMatrixRotationRollPitchYaw(m_pitch, m_yaw, 0);
            //m_at의 좌표를 변환한다.
            m_at = XMVector3TransformCoord(DEFAULT_FORWARD, m_rotation);
            //m_at을 그 머냐 그거 한다.
            m_at = XMVector3Normalize(m_at);

            XMMATRIX RotateYTempMatrix;
            RotateYTempMatrix = XMMatrixRotationY(m_yaw);

            m_cameraRight = XMVector3TransformCoord(DEFAULT_RIGHT, RotateYTempMatrix);
            m_cameraUp = XMVector3TransformCoord(m_cameraUp, RotateYTempMatrix);
            m_cameraForward = XMVector3TransformCoord(DEFAULT_FORWARD, RotateYTempMatrix);

            m_eye += m_moveLeftRight * m_cameraRight * m_travelSpeed;
            m_eye += m_moveBackForward * m_cameraForward * m_travelSpeed;
            m_eye += m_moveUpDown * m_cameraUp * m_travelSpeed;

            m_moveLeftRight = 0.0f;
            m_moveBackForward = 0.0f;
            m_moveUpDown = 0.0f;

            m_at = m_eye + m_at;

            m_view = XMMatrixLookAtLH(m_eye, m_at, m_up);
        };

    };

