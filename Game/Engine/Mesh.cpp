#include "pch.h"
#include "Mesh.h"
#include "Engine.h"
#include "Material.h"
#include "InstancingBuffer.h"

Mesh::Mesh() : Object(OBJECT_TYPE::MESH)
{
}

Mesh::~Mesh()
{
}

void Mesh::Init(const vector<Vertex>& vertexBuffer, const vector<uint32>& indexBuffer)
{
	CreateVertexBuffer(vertexBuffer);
	CreateIndexBuffer(indexBuffer);
}

void Mesh::Render(uint32 instanceCount)
{
	GRAPHICS_CMD_LIST->IASetVertexBuffers(0, 1, &_vertexBufferView); // Slot: (0~15)
	GRAPHICS_CMD_LIST->IASetIndexBuffer(&_indexBufferView);

	GEngine->GetGraphicsDescHeap()->CommitTable();

	// 그리기 예약
	
	// Vertex로 그리기
	//CMD_LIST->DrawInstanced(_vertexCount, 1, 0, 0);

	// Index로 그리기
	GRAPHICS_CMD_LIST->DrawIndexedInstanced(_indexCount, instanceCount, 0, 0, 0);
}

void Mesh::Render(shared_ptr<class InstancingBuffer>& buffer)
{
	D3D12_VERTEX_BUFFER_VIEW bufferViews[] = { _vertexBufferView, buffer->GetBufferView() };
	GRAPHICS_CMD_LIST->IASetVertexBuffers(0, 2, bufferViews);
	GRAPHICS_CMD_LIST->IASetIndexBuffer(&_indexBufferView);

	GEngine->GetGraphicsDescHeap()->CommitTable();

	GRAPHICS_CMD_LIST->DrawIndexedInstanced(_indexCount, buffer->GetCount(), 0, 0, 0);
}

void Mesh::CreateVertexBuffer(const vector<Vertex>& buffer)
{
	_vertexCount = static_cast<uint32>(buffer.size());
	uint32 bufferSize = _vertexCount * sizeof(Vertex);

	D3D12_HEAP_PROPERTIES heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

	DEVICE->CreateCommittedResource(
		&heapProperty,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&_vertexBuffer));

	void* vertexDataBuffer = nullptr;
	CD3DX12_RANGE readRange(0, 0);
	_vertexBuffer->Map(0, &readRange, &vertexDataBuffer);
	::memcpy(vertexDataBuffer, &buffer[0], bufferSize);
	_vertexBuffer->Unmap(0, nullptr);

	_vertexBufferView.BufferLocation = _vertexBuffer->GetGPUVirtualAddress();
	_vertexBufferView.StrideInBytes = sizeof(Vertex); // 정점 1개 크기
	_vertexBufferView.SizeInBytes = bufferSize; // 버퍼의 크기	
}

void Mesh::CreateIndexBuffer(const vector<uint32>& buffer)
{
	_indexCount = static_cast<uint32>(buffer.size());
	uint32 bufferSize = _indexCount * sizeof(uint32);

	D3D12_HEAP_PROPERTIES heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

	DEVICE->CreateCommittedResource(
		&heapProperty,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&_indexBuffer));

	void* indexDataBuffer = nullptr;
	CD3DX12_RANGE readRange(0, 0);
	_indexBuffer->Map(0, &readRange, &indexDataBuffer);
	::memcpy(indexDataBuffer, &buffer[0], bufferSize);
	_indexBuffer->Unmap(0, nullptr);

	_indexBufferView.BufferLocation = _indexBuffer->GetGPUVirtualAddress();
	_indexBufferView.Format = DXGI_FORMAT_R32_UINT;
	_indexBufferView.SizeInBytes = bufferSize;
}