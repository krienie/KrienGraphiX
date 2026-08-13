
#include "KGXDrawPackage.h"

namespace kgx
{
XXH64_hash_t hash(const rendering::KGXDrawStatePackage& drawStatePackage)
{
	XXH3_state_t* state = XXH3_createState();

	if (!state)
	{
		KGXLOG_CRITICAL("Error creating XXH3 state. Aborting.");
		//TODO(KL): Think about if this is the right way to handle this...
		abort();
	}

	XXH3_64bits_reset(state);

	XXH3_64bits_update(state, &drawStatePackage.viewport, sizeof(decltype(drawStatePackage.viewport)));
	XXH3_64bits_update(state, &drawStatePackage.pso, sizeof(decltype(drawStatePackage.pso)));
	XXH3_64bits_update(state, &drawStatePackage.depthStencilView, sizeof(decltype(drawStatePackage.depthStencilView)));
	XXH3_64bits_update(state, drawStatePackage.renderTargetViews.data(),
		sizeof(RHI::RHIResourceView*) * drawStatePackage.renderTargetViews.size());

	//TODO(KL): Perhaps also hash inputCBData?

	XXH64_hash_t result = XXH3_64bits_digest(state);
	XXH3_freeState(state);
	return result;
}
}
