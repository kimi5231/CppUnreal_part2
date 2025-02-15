#pragma once

// GPU와 정보를 주고받는 클래스
// 각종 객체 생성을 담당
class Device
{
public:
	void Init();

	ComPtr<IDXGIFactory> GetDXGI() { return _dxgi; }
	ComPtr<ID3D12Device> GetDevice() { return _device; }

private:
	ComPtr<ID3D12Debug> _debugController;
	ComPtr<IDXGIFactory> _dxgi;	// 화면 관련 기능들
	ComPtr<ID3D12Device> _device;	// 각종 객체 생성
};