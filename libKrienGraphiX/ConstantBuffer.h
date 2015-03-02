
#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <string>
#include <map>
#include <vector>


namespace kgx
{
	class ConstantBuffer
	{
		public:
			ConstantBuffer( _In_ ID3D11Device *dxDevice );
			~ConstantBuffer();

			ID3D11Buffer* getDxBuffer() const;
			HRESULT create( UINT sizeInBytes );
			void commit();
			bool hasVariable( const std::string &name ) const;
			void addVariableDefinition( const std::string &name, UINT offset, UINT size );
			/** Searches for a variable with the given name and updates its data when found. Returns true if the variable was found, false otherwise */
			bool updateVariable( const std::string &name, const void *var );

		private:
			struct VarPosition
			{
				VarPosition( UINT o, UINT s ) : offset(o), size(s) { }

				UINT offset;
				UINT size;
				//T Value;
			};

			ID3D11Device *dxDev;
			ID3D11DeviceContext *dxDevCont;
			ID3D11Buffer *dxBuffer;
			UINT bufferElementSize;

			bool dataChanged;
			std::map<std::string, VarPosition> variables;
			//std::vector<void*> rawData;
			UCHAR *rawData;

	};
}
