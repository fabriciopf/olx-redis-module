#include "redismodule.h"
#include "olx-ads.h"
#include <list>
#include <cstdlib>


bool compare_second_element_pair(const std::pair<RedisModuleCallReply*,int> &left, const std::pair<RedisModuleCallReply*,int> &right) {
    return left.second < right.second;
}

// TODO: return using JSON format
// in: lat, long, min distance, max distance
// out: an array of adID's sorted by price
int OlxGetAdsNearMeBetweenRadiuses_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
    //RedisModule_AutoMemory(ctx);

    RedisModuleCallReply* adsReply = RedisModule_Call(ctx, "GEORADIUS", "csssccc", "ads", argv[2], argv[1], argv[4], "m", "withcoord", "withdist");
    RedisModuleCallReply* adGeoReply = NULL;
    std::list<std::pair<RedisModuleCallReply*, int> > adsSortedSet;

	long long minDistance = 0;
	double adsIndex = 0;
    double adsFilteredCount = 0;
    double adDistance = 0;
    RedisModule_StringToLongLong(argv[3], &minDistance);
    //RedisModule_ReplyWithArray(ctx, REDISMODULE_POSTPONED_ARRAY_LEN);
    adGeoReply = RedisModule_CallReplyArrayElement(adsReply, adsIndex++);
    while (adGeoReply > 0) {
    	RedisModuleCallReply* adIDReply = RedisModule_CallReplyArrayElement(adGeoReply, 0);
    	RedisModuleCallReply* adDistanceReply = RedisModule_CallReplyArrayElement(adGeoReply, 1);
    	RedisModuleString* adDistanceString = RedisModule_CreateStringFromCallReply(adDistanceReply);
        RedisModule_StringToDouble(adDistanceString, &adDistance);
    	if (adDistance > minDistance) {
            adsFilteredCount++;
            RedisModuleString* adIDString = RedisModule_CreateStringFromCallReply(adIDReply);

			RedisModuleCallReply* adReply = RedisModule_Call(ctx, "HGETALL", "s", adIDString);
            if (adReply) {
                RedisModuleCallReply* adPriceReply = RedisModule_CallReplyArrayElement(adReply, 1);
                const char* adPrice = RedisModule_CallReplyStringPtr(adPriceReply, NULL);
                adsSortedSet.push_back(std::pair<RedisModuleCallReply*, int>(adGeoReply, atoi(adPrice)));

                //RedisModule_FreeCallReply(adGeoReply);
                //RedisModule_FreeCallReply(adPriceReply);
            }
            //RedisModule_FreeCallReply(adGeoReply);

            //RedisModule_FreeString(ctx, adIDString);
    	}
        //RedisModule_FreeCallReply(adGeoReply);
        adGeoReply = RedisModule_CallReplyArrayElement(adsReply, adsIndex++);
        //RedisModule_FreeString(ctx, adDistanceString);
        //RedisModule_FreeCallReply(adIDReply);
        //RedisModule_FreeCallReply(adDistanceReply);
    }
    //RedisModule_FreeCallReply(adGeoReply);
    //RedisModule_ReplySetArrayLength(ctx, adsFilteredCount);

    adsSortedSet.sort(compare_second_element_pair);
    RedisModule_ReplyWithArray(ctx, adsSortedSet.size());
    std::list<std::pair<RedisModuleCallReply*, int> >::iterator it;
	for (it = adsSortedSet.begin(); it != adsSortedSet.end(); ++it) {
		RedisModule_ReplyWithArray(ctx, 2);
		RedisModule_ReplyWithCallReply(ctx, it->first);
        RedisModule_ReplyWithDouble(ctx, it->second);
    }
    //RedisModule_FreeCallReply(adsReply);
    //RedisModule_ReplyWithNull(ctx);

    return REDISMODULE_OK;
}

int RedisModule_OnLoad(RedisModuleCtx *ctx) {
    if (RedisModule_Init(ctx,"olxads",1,REDISMODULE_APIVER_1) == REDISMODULE_ERR) 
    	return REDISMODULE_ERR;

    if (RedisModule_CreateCommand(ctx,"olxads.GetAdsNearMeBetweenRadiuses", OlxGetAdsNearMeBetweenRadiuses_RedisCommand, "readonly", 1, 1, 1) == REDISMODULE_ERR)
        return REDISMODULE_OK;

    return REDISMODULE_OK;
}
