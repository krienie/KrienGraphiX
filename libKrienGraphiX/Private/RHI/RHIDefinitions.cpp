
#include "RHIDefinitions.h"

namespace kgx
{
XXH64_hash_t hash(const RHI::RHIGraphicsPipelineStateDescriptor& psoDescriptor)
{
	XXH3_state_t* state = XXH3_createState();

	if (!state)
	{
		KGXLOG_CRITICAL("Error creating XXH3 state. Aborting.");
		//TODO(KL): Think about if this is the right way to handle this...
		abort();
	}

	XXH3_64bits_reset(state);

	XXH3_64bits_update(state, &psoDescriptor.vs, sizeof(decltype(psoDescriptor.vs)));
	XXH3_64bits_update(state, &psoDescriptor.ps, sizeof(decltype(psoDescriptor.ps)));
	XXH3_64bits_update(state, &psoDescriptor.ds, sizeof(decltype(psoDescriptor.ds)));
	XXH3_64bits_update(state, &psoDescriptor.hs, sizeof(decltype(psoDescriptor.hs)));
	XXH3_64bits_update(state, &psoDescriptor.gs, sizeof(decltype(psoDescriptor.gs)));

	XXH3_64bits_update(state, &psoDescriptor.primitiveTopology, sizeof(decltype(psoDescriptor.primitiveTopology)));

	XXH3_64bits_update(state, &psoDescriptor.numRenderTargets, sizeof(decltype(psoDescriptor.numRenderTargets)));

	for (int i = 0; i < psoDescriptor.numRenderTargets; i++)
	{
		XXH3_64bits_update(state, psoDescriptor.renderTargetFormats, sizeof(decltype(psoDescriptor.renderTargetFormats)));
	}

	XXH3_64bits_update(state, &psoDescriptor.depthStencilFormat, sizeof(decltype(psoDescriptor.depthStencilFormat)));

	XXH64_hash_t result = XXH3_64bits_digest(state);
	XXH3_freeState(state);
	return result;
}
}
