#ifndef _DEVICE_H_
#define _DEVICE_H_

#pragma comment(lib, "d3d11.lib")

#include <d3d11.h>
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;


//デバイスクラス
class Device {
private:
	ComPtr<ID3D11Device> m_device;
	ComPtr<ID3D11DeviceContext> m_context;

public:
	Device() {}
	~Device() {}

	bool Initialize() {
		HRESULT res;

		res = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, NULL, 0, D3D11_SDK_VERSION, m_device.ReleaseAndGetAddressOf(),
			NULL, m_context.ReleaseAndGetAddressOf());
		if (FAILED(res)) {
			MessageBox(NULL, "failed creating device.", "Device.cpp", MB_OK);
			return false;
		}

		return true;
	}

	ComPtr<ID3D11Device> GetDevice() { return m_device; }
	ComPtr<ID3D11DeviceContext> GetContext() { return m_context; }
};


//デバイス使用者基底クラス
class DeviceUser {
protected:
	static ComPtr<ID3D11Device> m_device;
	static ComPtr<ID3D11DeviceContext> m_context;

public:
	DeviceUser() {}
	~DeviceUser() {}

	static bool Initialize() {
		HRESULT res;

		res = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, NULL, 0, D3D11_SDK_VERSION, m_device.ReleaseAndGetAddressOf(),
			NULL, m_context.ReleaseAndGetAddressOf());
		if (FAILED(res)) {
			MessageBox(NULL, "failed creating device.", "Device.cpp", MB_OK);
			return false;
		}

		return true;
	}
};



#endif