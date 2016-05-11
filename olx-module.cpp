#include "redismodule.h"
#include <stdlib.h>
#include <vector>

// TODO: return using JSON format

struct sort_pred {
    bool operator()(const std::pair<int,double> &left, const std::pair<int,double> &right) {
        return left.second < right.second;
    }
};

// in: lat, long, min distance, max distance
// out: an array of adID's sorted by price
int OlxGetAdsNearMeBetweenRadiuses_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
    RedisModuleCallReply* adsReply = RedisModule_Call(ctx, "GEORADIUS", "ads %d %d %d m withcoord withdist", argv[0], argv[1], argv[3]);
    RedisModuleCallReply* adReply = NULL;
    std::vector<std::pair<int, double> > adsSortedSet;

	long long minDistance = 0;
    RedisModule_StringToLongLong(argv[2], &minDistance);
	int adsIndex = 0;
    while ((adReply = RedisModule_CallReplyArrayElement(adsReply, adsIndex++))) {
    	RedisModuleCallReply* adIDReply = RedisModule_CallReplyArrayElement(adReply, 0);
    	RedisModuleCallReply* adDistanceReply = RedisModule_CallReplyArrayElement(adReply, 1);
        int adID = RedisModule_CallReplyInteger(adIDReply);
    	int adDistance = RedisModule_CallReplyInteger(adDistanceReply);
    	if (adDistance > minDistance) {
			RedisModuleCallReply* adPriceReply = RedisModule_Call(ctx, "HGET", "%f price", &adID);
    		adsSortedSet.push_back(std::pair<int, double>(adID, RedisModule_CallReplyInteger(adPriceReply)));
    	}
    }

    std::sort(adsSortedSet.begin(), adsSortedSet.end(), sort_pred());
    RedisModule_ReplyWithArray(ctx, adsSortedSet.size());
    std::vector<std::pair<int, double> >::iterator it;
	for (it = adsSortedSet.begin(); it != adsSortedSet.end(); ++it) {
		RedisModule_ReplyWithArray(ctx, 2);
		RedisModule_ReplyWithLongLong(ctx, it->first);
		RedisModule_ReplyWithDouble(ctx, it->second);
    }

    //RedisModule_FreeCallReply_Rec(adsReply, 1);
    	
    return REDISMODULE_OK;
}

int RedisModule_OnLoad(RedisModuleCtx *ctx) {
    if (RedisModule_Init(ctx,"OlxAds",1,REDISMODULE_APIVER_1) == REDISMODULE_ERR) 
    	return REDISMODULE_ERR;

    if (RedisModule_CreateCommand(ctx,"OlxAds.GetAdsNearMeBetweenRadiuses", OlxGetAdsNearMeBetweenRadiuses_RedisCommand, "readonly", 0, 0, 0) == REDISMODULE_ERR)
        return REDISMODULE_OK;

    return REDISMODULE_OK;
}
