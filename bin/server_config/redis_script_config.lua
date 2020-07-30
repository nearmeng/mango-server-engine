RedisScriptConfig = {
    scripts = 
    {
        [recTest] = {
            szScript= [[
                local r

                r= redis.call('EXISTS', KEYS[1])
                if r== 0 then
                    redis.call('SET', KEYS[1], ARGV[1])
                end

                return 123
            ]],
        };
    };
};