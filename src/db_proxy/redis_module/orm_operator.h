#ifndef _ORM_OPERATOR_H_
#define _ORM_OPERATOR_H_

#include "redis_mgr.h"
#include "google/protobuf/message.h"

template <typename Model>
class COrmOperator
{
public:
    COrmOperator() {};
    ~COrmOperator() {};

    Model get(CRedisCli* pRedisCli, const char* pcszKey);
    BOOL  set(CRedisCli* pRedisCli, Model& rModel, int32_t nCmdID, const char* pUserData, size_t dwDataLen);
    BOOL  del(CRedisCli* pRedisCli, const char* pcszKey);

    BOOL set_part(CRedisCli* pRedisCli, const char* pcszFieldName, const void* pcszValue, size_t dwSize);
    const void* get_part(CRedisCli* pRedisCli, const void* pcszFieldName, size_t& dwSize);

private:
    BOOL _push_arg(const char* pArgv, size_t dwArgvSize, REDIS_ARG_DATA& rArgData);
    BOOL _set_primary_key(const google::protobuf::Reflection* pRef, Model& rModel, const google::protobuf::FieldDescriptor* pFieldDesc, char* pszPrimaryKey);
};

template<typename Model>
BOOL COrmOperator<Model>::_push_arg(const char* pArgv, size_t dwArgvSize, REDIS_ARG_DATA& rArgData)
{
    int32_t nRetCode = 0;

    LOG_PROCESS_ERROR(nArgc < MAX_MODEL_ARG_COUNT);

    rArgData.pArgv[nArgc] = pArgv;
    rArgData.dwArgvSize[nArgc] = dwArgvSize;
    rArgData.nArgc++;

    return TRUE;
Exit0:
    return FALSE;
}
    
template<typename Model>
BOOL COrmOperator<Model>::_set_primary_key(const google::protobuf::Reflection* pRef, Model& rModel,
    const google::protobuf::FieldDescriptor* pFieldDesc, char* pszPrimaryKey)
{
    int32_t nRetCode = 0;

    LOG_PROCESS_ERROR(pRef && pFieldDesc && pszPrimaryKey);

    switch (pFieldDesc->cpp_type())
    {
	    case google::protobuf::FieldDescriptor::CPPTYPE_STRING:
            snprintf(pszPrimaryKey, MAX_PRIMARY_KEY_LEN, "%s:%s:%s", typeid(Model).name().c_str(), pFieldDesc->name().c_str(), pRef->GetString(rModel, pFieldDesc).c_str());
            break;

	    case google::protobuf::FieldDescriptor::CPPTYPE_INT32:
            snprintf(pszPrimaryKey, MAX_PRIMARY_KEY_LEN, "%s:%s:%d", typeid(Model).name().c_str(), pFieldDesc->name().c_str(), pRef->GetInt32(rModel, pFieldDesc));
            break;
	    
        case google::protobuf::FieldDescriptor::CPPTYPE_UINT32:
            snprintf(pszPrimaryKey, MAX_PRIMARY_KEY_LEN, "%s:%s:%u", typeid(Model).name().c_str(), pFieldDesc->name().c_str(), pRef->GetUInt32(rModel, pFieldDesc));
            break;
        
        case google::protobuf::FieldDescriptor::CPPTYPE_INT64:
            snprintf(pszPrimaryKey, MAX_PRIMARY_KEY_LEN, "%s:%s:%lld", typeid(Model).name().c_str(), pFieldDesc->name().c_str(), pRef->GetInt64(rModel, pFieldDesc));
            break;
	        	
        case google::protobuf::FieldDescriptor::CPPTYPE_UINT64:
            snprintf(pszPrimaryKey, MAX_PRIMARY_KEY_LEN, "%s:%s:%llu", typeid(Model).name().c_str(), pFieldDesc->name().c_str(), pRef->GetUInt64(rModel, pFieldDesc));
            break;

        default:
            CRI("primary key type is not support");
            LOG_PROCESS_ERROR(FALSE);
    }

    return TRUE;
Exit0:
    return FALSE;
}

template<typename Model>
Model COrmOperator<Model>::get(CRedisCli* pRedisCli, const char* pcszKey)
{
    Model OrmModel;
    int32_t nRetCode = 0;
    std::string sModelName = typeid(Model).name();
	const google::protobuf::Descriptor* pDescriptor = NULL;
	const google::protobuf::Message* pType = NULL;
	google::protobuf::Message* pModelMessage = NULL;
	
    transform(sModelName.begin(), sModelName.end(), sModelName.begin(), ::toupper);

	pDescriptor = google::protobuf::DescriptorPool::generated_pool()->FindMessageTypeByName(sModelName);
	LOG_PROCESS_ERROR(pDescriptor);

	pType = google::protobuf::MessageFactory::generated_factory()->GetPrototype(pDescriptor);
	LOG_PROCESS_ERROR(pType);

	pModelMessage = pType->New();
	LOG_PROCESS_ERROR(pModelMessage);

    return M;
}

template<typename Model>
BOOL COrmOperator<Model>::set(CRedisCli* pRedisCli, Model& rModel, int32_t nCmdID, 
                         const char* pUserData, size_t dwDataLen)
{
    ARG_DATA stArgData = { 0 };
    char szPriMaryKey[MAX_PRIMARY_KEY_LEN];
	const google::protobuf::FieldDescriptor* pFieldDes = NULL;
	const google::protobuf::Descriptor* pDescriptor = rModel.GetDescriptor();
	const google::protobuf::Reflection* pRef = rModel.GetReflection();
    std::string sPrimaryKey = pDescriptor->options().GetExtension(primary_key);

    //command
    LOG_PROCESS_ERROR(_push_arg("HMSET", 5, stArgData));

    //primary key
    pFieldDes = pDescriptor->FindFieldByName(sPrimaryKey);
    LOG_PROCESS_ERROR(pFieldDes);

    nRetCode = _set_primary_key(pRef, pFieldDes, szPriMaryKey);
    LOG_PROCESS_ERROR(nRetCode);
    LOG_PROCESS_ERROR(_push_arg(szPriMaryKey, strlen(szPriMaryKey), stArgData));

    //param
	for (int32_t i = 0; i < pDescriptor->field_count(); ++i)
	{
		pFieldDes = pDescriptor->field(i);
        _push_arg(pFieldDes->name().c_str(), pFieldDes->name().size(), stArgData);

		if (pFieldDes->is_repeated())
		{
            LOG_PROCESS_ERROR_DETAIL(FALSE, "repeated is not supported in Model Desc");
		}
		else
		{
            char* pFieldArg = NULL;
            size_t dwFieldArgLen = 0;

	        switch (pFieldDes->cpp_type())
	        {
	        	case google::protobuf::FieldDescriptor::CPPTYPE_STRING:
	        	{
	        		std::string sValue = pRef->GetString(rModel, pFieldDes);
                    pFieldArg = malloc(sValue.size());
                    LOG_PROCESS_ERROR(pFieldArg);

                    memcpy(pFieldArg, sValue.c_str(), sValue.size());
                    dwFieldArgLen = sValue.size();
	        		break;
	        	}
	        	case google::protobuf::FieldDescriptor::CPPTYPE_INT32:
	        	{
	        		int32_t nValue = pRef->GetInt32(rModel, pFieldDes);
                    pFieldArg = malloc(sizeof(int32_t));
                    LOG_PROCESS_ERROR(pFieldArg);

                    *(int32_t*)pFieldArg = nValue;
                    dwFieldArgLen = sizeof(int32_t);
	        		break;
	        	}
	        	case google::protobuf::FieldDescriptor::CPPTYPE_BOOL:
	        	{
	        		int32_t nValue = pRef->GetBool(rModel, pFieldDes);
                    pFieldArg = malloc(sizeof(int32_t));
                    LOG_PROCESS_ERROR(pFieldArg);

                    *(int32_t*)pFieldArg = nValue;
                    dwFieldArgLen = sizeof(int32_t);
	        		break;
	        	}
	        	case google::protobuf::FieldDescriptor::CPPTYPE_ENUM:
	        	{
	        		int32_t nValue = pRef->GetEnum(rModel, pFieldDes)->number();
                    pFieldArg = malloc(sizeof(int32_t));
                    LOG_PROCESS_ERROR(pFieldArg);

                    *(int32_t*)pFieldArg = nValue;
                    dwFieldArgLen = sizeof(int32_t);
	        		break;
	        	}
	        	case google::protobuf::FieldDescriptor::CPPTYPE_FLOAT:
	        	{
	        		float fValue = pRef->GetFloat(rModel, pFieldDes);
                    pFieldArg = malloc(sizeof(float));
                    LOG_PROCESS_ERROR(pFieldArg);

                    *(float*)pFieldArg = nValue;
                    dwFieldArgLen = sizeof(float);
	        		break;
	        	}
	        	case google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE:
	        	{
	        		double dValue = pRef->GetDouble(rModel, pFieldDes);
                    pFieldArg = malloc(sizeof(double));
                    LOG_PROCESS_ERROR(pFieldArg);

                    *(double*)pFieldArg = dValue;
                    dwFieldArgLen = sizeof(double);
	        		break;
	        	}
	        	case google::protobuf::FieldDescriptor::CPPTYPE_UINT64:
	        	{
	        		uint64_t qwValue = pRef->GetUInt64(rModel, pFieldDes);
                    pFieldArg = malloc(sizeof(uint64_t));
                    LOG_PROCESS_ERROR(pFieldArg);

                    *(uint64_t*)pFieldArg = dValue;
                    dwFieldArgLen = sizeof(uint64_t);
	        		break;
	        	}
	        	case google::protobuf::FieldDescriptor::CPPTYPE_INT64:
	        	{
	        		int64_t llValue = pRef->GetInt64(rModel, pFieldDes);
                    pFieldArg = malloc(sizeof(int64_t));
                    LOG_PROCESS_ERROR(pFieldArg);

                    *(int64_t*)pFieldArg = llValue;
                    dwFieldArgLen = sizeof(int64_t);
	        		break;
	        	}
	        	case google::protobuf::FieldDescriptor::CPPTYPE_UINT32:
	        	{
	        		uint32_t dwValue = pRef->GetUInt32(rModel, pFieldDes);
                    pFieldArg = malloc(sizeof(uint32_t));
                    LOG_PROCESS_ERROR(pFieldArg);

                    *(uint32_t*)pFieldArg = dwValue;
                    dwFieldArgLen = sizeof(uint32_t);
	        		break;
	        	}
	        	default:
	        	{
	        		LOG_PROCESS_ERROR(FALSE);
	        		break;
	        	}
	        }
            LOG_PROCESS_ERROR(_push_arg(pFieldArg, stArgData));
		}
	}

    pRedisCli->command();

    return TRUE;
Exit0:
    return FALSE;
}

template<typename Model>
BOOL COrmOperator<Model>::del(CRedisCli* pRedisCli, const char* pcszKey)
{
    return TRUE;
}

template<typename Model>
BOOL COrmOperator<Model>::set_part(CRedisCli* pRedisCli, const char* pcszFieldName, const void* pcszValue, size_t dwSize)
{
    return TRUE;
}

template<typename Model>
const void* COrmOperator<Model>::get_part(CRedisCli* pRedisCli, const void* pcszFieldName, size_t& dwSize)
{
    return TRUE;
}

#endif