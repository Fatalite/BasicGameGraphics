#include "Light/PointLight.h"

#include "Renderer/DataTypes.h"

namespace library
{

    PointLight::PointLight(_In_ const XMFLOAT4& position, _In_ const XMFLOAT4& color, _In_ FLOAT attenuationDistance):
        m_position(position), m_color(color), m_at({0.0f,0.0f,0.0f,1.0f}), m_attenuationDistance(attenuationDistance){};
    const XMFLOAT4& PointLight::GetPosition() const {
        return m_position;
    };
    const XMFLOAT4& PointLight::GetColor() const {
        return m_color;
    };

    const XMMATRIX& PointLight::GetViewMatrix() const {
        return m_view;
    }
    const XMMATRIX& PointLight::GetProjectionMatrix() const {
        return m_projection;
    }

    void  PointLight::Initialize(_In_ UINT uWidth, _In_ UINT uHeight) {

        m_projection = XMMatrixPerspectiveFovLH(
            XM_PIDIV4, 
            static_cast<FLOAT>(uWidth) / static_cast<FLOAT>(uHeight), 
            0.01f, 
            1000.0f);
    }


    void PointLight::Update(_In_ FLOAT deltaTime)
    {
        UNREFERENCED_PARAMETER(deltaTime);

        m_eye = XMLoadFloat4(&m_position);
        //m_at = XMVECTORF32(0.0, 0.0, 0.0, 1.0f);
        m_up = DEFAULT_UP;
        //Set m_view(by above values)
        //m_at = m_eye + m_at;
        m_view = XMMatrixLookAtLH(m_eye, m_at, m_up);
    }

    FLOAT PointLight::GetAttenuationDistance() const {
        return m_attenuationDistance;
    }
}