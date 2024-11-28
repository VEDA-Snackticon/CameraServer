/**
 *
 *  CameraEvent.cc
 *  DO NOT EDIT. This file is generated by drogon_ctl
 *
 */

#include "CameraEvent.h"
#include <drogon/utils/Utilities.h>
#include <string>

using namespace drogon;
using namespace drogon::orm;
using namespace drogon_model::veda4;

const std::string CameraEvent::Cols::_transaction_id = "transaction_id";
const std::string CameraEvent::Cols::_event_camera_id = "event_camera_id";
const std::string CameraEvent::Cols::_time = "time";
const std::string CameraEvent::primaryKeyName = "transaction_id";
const bool CameraEvent::hasPrimaryKey = true;
const std::string CameraEvent::tableName = "camera_event";

const std::vector<typename CameraEvent::MetaData> CameraEvent::metaData_={
{"transaction_id","std::string","char(36)",0,0,1,1},
{"event_camera_id","int64_t","bigint",8,0,0,0},
{"time","::trantor::Date","timestamp",0,0,0,0}
};
const std::string &CameraEvent::getColumnName(size_t index) noexcept(false)
{
    assert(index < metaData_.size());
    return metaData_[index].colName_;
}
CameraEvent::CameraEvent(const Row &r, const ssize_t indexOffset) noexcept
{
    if(indexOffset < 0)
    {
        if(!r["transaction_id"].isNull())
        {
            transactionId_=std::make_shared<std::string>(r["transaction_id"].as<std::string>());
        }
        if(!r["event_camera_id"].isNull())
        {
            eventCameraId_=std::make_shared<int64_t>(r["event_camera_id"].as<int64_t>());
        }
        if(!r["time"].isNull())
        {
            auto timeStr = r["time"].as<std::string>();
            struct tm stm;
            memset(&stm,0,sizeof(stm));
            auto p = strptime(timeStr.c_str(),"%Y-%m-%d %H:%M:%S",&stm);
            time_t t = mktime(&stm);
            size_t decimalNum = 0;
            if(p)
            {
                if(*p=='.')
                {
                    std::string decimals(p+1,&timeStr[timeStr.length()]);
                    while(decimals.length()<6)
                    {
                        decimals += "0";
                    }
                    decimalNum = (size_t)atol(decimals.c_str());
                }
                time_=std::make_shared<::trantor::Date>(t*1000000+decimalNum);
            }
        }
    }
    else
    {
        size_t offset = (size_t)indexOffset;
        if(offset + 3 > r.size())
        {
            LOG_FATAL << "Invalid SQL result for this model";
            return;
        }
        size_t index;
        index = offset + 0;
        if(!r[index].isNull())
        {
            transactionId_=std::make_shared<std::string>(r[index].as<std::string>());
        }
        index = offset + 1;
        if(!r[index].isNull())
        {
            eventCameraId_=std::make_shared<int64_t>(r[index].as<int64_t>());
        }
        index = offset + 2;
        if(!r[index].isNull())
        {
            auto timeStr = r[index].as<std::string>();
            struct tm stm;
            memset(&stm,0,sizeof(stm));
            auto p = strptime(timeStr.c_str(),"%Y-%m-%d %H:%M:%S",&stm);
            time_t t = mktime(&stm);
            size_t decimalNum = 0;
            if(p)
            {
                if(*p=='.')
                {
                    std::string decimals(p+1,&timeStr[timeStr.length()]);
                    while(decimals.length()<6)
                    {
                        decimals += "0";
                    }
                    decimalNum = (size_t)atol(decimals.c_str());
                }
                time_=std::make_shared<::trantor::Date>(t*1000000+decimalNum);
            }
        }
    }

}

CameraEvent::CameraEvent(const Json::Value &pJson, const std::vector<std::string> &pMasqueradingVector) noexcept(false)
{
    if(pMasqueradingVector.size() != 3)
    {
        LOG_ERROR << "Bad masquerading vector";
        return;
    }
    if(!pMasqueradingVector[0].empty() && pJson.isMember(pMasqueradingVector[0]))
    {
        dirtyFlag_[0] = true;
        if(!pJson[pMasqueradingVector[0]].isNull())
        {
            transactionId_=std::make_shared<std::string>(pJson[pMasqueradingVector[0]].asString());
        }
    }
    if(!pMasqueradingVector[1].empty() && pJson.isMember(pMasqueradingVector[1]))
    {
        dirtyFlag_[1] = true;
        if(!pJson[pMasqueradingVector[1]].isNull())
        {
            eventCameraId_=std::make_shared<int64_t>((int64_t)pJson[pMasqueradingVector[1]].asInt64());
        }
    }
    if(!pMasqueradingVector[2].empty() && pJson.isMember(pMasqueradingVector[2]))
    {
        dirtyFlag_[2] = true;
        if(!pJson[pMasqueradingVector[2]].isNull())
        {
            auto timeStr = pJson[pMasqueradingVector[2]].asString();
            struct tm stm;
            memset(&stm,0,sizeof(stm));
            auto p = strptime(timeStr.c_str(),"%Y-%m-%d %H:%M:%S",&stm);
            time_t t = mktime(&stm);
            size_t decimalNum = 0;
            if(p)
            {
                if(*p=='.')
                {
                    std::string decimals(p+1,&timeStr[timeStr.length()]);
                    while(decimals.length()<6)
                    {
                        decimals += "0";
                    }
                    decimalNum = (size_t)atol(decimals.c_str());
                }
                time_=std::make_shared<::trantor::Date>(t*1000000+decimalNum);
            }
        }
    }
}

CameraEvent::CameraEvent(const Json::Value &pJson) noexcept(false)
{
    if(pJson.isMember("transaction_id"))
    {
        dirtyFlag_[0]=true;
        if(!pJson["transaction_id"].isNull())
        {
            transactionId_=std::make_shared<std::string>(pJson["transaction_id"].asString());
        }
    }
    if(pJson.isMember("event_camera_id"))
    {
        dirtyFlag_[1]=true;
        if(!pJson["event_camera_id"].isNull())
        {
            eventCameraId_=std::make_shared<int64_t>((int64_t)pJson["event_camera_id"].asInt64());
        }
    }
    if(pJson.isMember("time"))
    {
        dirtyFlag_[2]=true;
        if(!pJson["time"].isNull())
        {
            auto timeStr = pJson["time"].asString();
            struct tm stm;
            memset(&stm,0,sizeof(stm));
            auto p = strptime(timeStr.c_str(),"%Y-%m-%d %H:%M:%S",&stm);
            time_t t = mktime(&stm);
            size_t decimalNum = 0;
            if(p)
            {
                if(*p=='.')
                {
                    std::string decimals(p+1,&timeStr[timeStr.length()]);
                    while(decimals.length()<6)
                    {
                        decimals += "0";
                    }
                    decimalNum = (size_t)atol(decimals.c_str());
                }
                time_=std::make_shared<::trantor::Date>(t*1000000+decimalNum);
            }
        }
    }
}

void CameraEvent::updateByMasqueradedJson(const Json::Value &pJson,
                                            const std::vector<std::string> &pMasqueradingVector) noexcept(false)
{
    if(pMasqueradingVector.size() != 3)
    {
        LOG_ERROR << "Bad masquerading vector";
        return;
    }
    if(!pMasqueradingVector[0].empty() && pJson.isMember(pMasqueradingVector[0]))
    {
        if(!pJson[pMasqueradingVector[0]].isNull())
        {
            transactionId_=std::make_shared<std::string>(pJson[pMasqueradingVector[0]].asString());
        }
    }
    if(!pMasqueradingVector[1].empty() && pJson.isMember(pMasqueradingVector[1]))
    {
        dirtyFlag_[1] = true;
        if(!pJson[pMasqueradingVector[1]].isNull())
        {
            eventCameraId_=std::make_shared<int64_t>((int64_t)pJson[pMasqueradingVector[1]].asInt64());
        }
    }
    if(!pMasqueradingVector[2].empty() && pJson.isMember(pMasqueradingVector[2]))
    {
        dirtyFlag_[2] = true;
        if(!pJson[pMasqueradingVector[2]].isNull())
        {
            auto timeStr = pJson[pMasqueradingVector[2]].asString();
            struct tm stm;
            memset(&stm,0,sizeof(stm));
            auto p = strptime(timeStr.c_str(),"%Y-%m-%d %H:%M:%S",&stm);
            time_t t = mktime(&stm);
            size_t decimalNum = 0;
            if(p)
            {
                if(*p=='.')
                {
                    std::string decimals(p+1,&timeStr[timeStr.length()]);
                    while(decimals.length()<6)
                    {
                        decimals += "0";
                    }
                    decimalNum = (size_t)atol(decimals.c_str());
                }
                time_=std::make_shared<::trantor::Date>(t*1000000+decimalNum);
            }
        }
    }
}

void CameraEvent::updateByJson(const Json::Value &pJson) noexcept(false)
{
    if(pJson.isMember("transaction_id"))
    {
        if(!pJson["transaction_id"].isNull())
        {
            transactionId_=std::make_shared<std::string>(pJson["transaction_id"].asString());
        }
    }
    if(pJson.isMember("event_camera_id"))
    {
        dirtyFlag_[1] = true;
        if(!pJson["event_camera_id"].isNull())
        {
            eventCameraId_=std::make_shared<int64_t>((int64_t)pJson["event_camera_id"].asInt64());
        }
    }
    if(pJson.isMember("time"))
    {
        dirtyFlag_[2] = true;
        if(!pJson["time"].isNull())
        {
            auto timeStr = pJson["time"].asString();
            struct tm stm;
            memset(&stm,0,sizeof(stm));
            auto p = strptime(timeStr.c_str(),"%Y-%m-%d %H:%M:%S",&stm);
            time_t t = mktime(&stm);
            size_t decimalNum = 0;
            if(p)
            {
                if(*p=='.')
                {
                    std::string decimals(p+1,&timeStr[timeStr.length()]);
                    while(decimals.length()<6)
                    {
                        decimals += "0";
                    }
                    decimalNum = (size_t)atol(decimals.c_str());
                }
                time_=std::make_shared<::trantor::Date>(t*1000000+decimalNum);
            }
        }
    }
}

const std::string &CameraEvent::getValueOfTransactionId() const noexcept
{
    static const std::string defaultValue = std::string();
    if(transactionId_)
        return *transactionId_;
    return defaultValue;
}
const std::shared_ptr<std::string> &CameraEvent::getTransactionId() const noexcept
{
    return transactionId_;
}
void CameraEvent::setTransactionId(const std::string &pTransactionId) noexcept
{
    transactionId_ = std::make_shared<std::string>(pTransactionId);
    dirtyFlag_[0] = true;
}
void CameraEvent::setTransactionId(std::string &&pTransactionId) noexcept
{
    transactionId_ = std::make_shared<std::string>(std::move(pTransactionId));
    dirtyFlag_[0] = true;
}
const typename CameraEvent::PrimaryKeyType & CameraEvent::getPrimaryKey() const
{
    assert(transactionId_);
    return *transactionId_;
}

const int64_t &CameraEvent::getValueOfEventCameraId() const noexcept
{
    static const int64_t defaultValue = int64_t();
    if(eventCameraId_)
        return *eventCameraId_;
    return defaultValue;
}
const std::shared_ptr<int64_t> &CameraEvent::getEventCameraId() const noexcept
{
    return eventCameraId_;
}
void CameraEvent::setEventCameraId(const int64_t &pEventCameraId) noexcept
{
    eventCameraId_ = std::make_shared<int64_t>(pEventCameraId);
    dirtyFlag_[1] = true;
}
void CameraEvent::setEventCameraIdToNull() noexcept
{
    eventCameraId_.reset();
    dirtyFlag_[1] = true;
}

const ::trantor::Date &CameraEvent::getValueOfTime() const noexcept
{
    static const ::trantor::Date defaultValue = ::trantor::Date();
    if(time_)
        return *time_;
    return defaultValue;
}
const std::shared_ptr<::trantor::Date> &CameraEvent::getTime() const noexcept
{
    return time_;
}
void CameraEvent::setTime(const ::trantor::Date &pTime) noexcept
{
    time_ = std::make_shared<::trantor::Date>(pTime);
    dirtyFlag_[2] = true;
}
void CameraEvent::setTimeToNull() noexcept
{
    time_.reset();
    dirtyFlag_[2] = true;
}

void CameraEvent::updateId(const uint64_t id)
{
}

const std::vector<std::string> &CameraEvent::insertColumns() noexcept
{
    static const std::vector<std::string> inCols={
        "transaction_id",
        "event_camera_id",
        "time"
    };
    return inCols;
}

void CameraEvent::outputArgs(drogon::orm::internal::SqlBinder &binder) const
{
    if(dirtyFlag_[0])
    {
        if(getTransactionId())
        {
            binder << getValueOfTransactionId();
        }
        else
        {
            binder << nullptr;
        }
    }
    if(dirtyFlag_[1])
    {
        if(getEventCameraId())
        {
            binder << getValueOfEventCameraId();
        }
        else
        {
            binder << nullptr;
        }
    }
    if(dirtyFlag_[2])
    {
        if(getTime())
        {
            binder << getValueOfTime();
        }
        else
        {
            binder << nullptr;
        }
    }
}

const std::vector<std::string> CameraEvent::updateColumns() const
{
    std::vector<std::string> ret;
    if(dirtyFlag_[0])
    {
        ret.push_back(getColumnName(0));
    }
    if(dirtyFlag_[1])
    {
        ret.push_back(getColumnName(1));
    }
    if(dirtyFlag_[2])
    {
        ret.push_back(getColumnName(2));
    }
    return ret;
}

void CameraEvent::updateArgs(drogon::orm::internal::SqlBinder &binder) const
{
    if(dirtyFlag_[0])
    {
        if(getTransactionId())
        {
            binder << getValueOfTransactionId();
        }
        else
        {
            binder << nullptr;
        }
    }
    if(dirtyFlag_[1])
    {
        if(getEventCameraId())
        {
            binder << getValueOfEventCameraId();
        }
        else
        {
            binder << nullptr;
        }
    }
    if(dirtyFlag_[2])
    {
        if(getTime())
        {
            binder << getValueOfTime();
        }
        else
        {
            binder << nullptr;
        }
    }
}
Json::Value CameraEvent::toJson() const
{
    Json::Value ret;
    if(getTransactionId())
    {
        ret["transaction_id"]=getValueOfTransactionId();
    }
    else
    {
        ret["transaction_id"]=Json::Value();
    }
    if(getEventCameraId())
    {
        ret["event_camera_id"]=(Json::Int64)getValueOfEventCameraId();
    }
    else
    {
        ret["event_camera_id"]=Json::Value();
    }
    if(getTime())
    {
        ret["time"]=getTime()->toDbStringLocal();
    }
    else
    {
        ret["time"]=Json::Value();
    }
    return ret;
}

Json::Value CameraEvent::toMasqueradedJson(
    const std::vector<std::string> &pMasqueradingVector) const
{
    Json::Value ret;
    if(pMasqueradingVector.size() == 3)
    {
        if(!pMasqueradingVector[0].empty())
        {
            if(getTransactionId())
            {
                ret[pMasqueradingVector[0]]=getValueOfTransactionId();
            }
            else
            {
                ret[pMasqueradingVector[0]]=Json::Value();
            }
        }
        if(!pMasqueradingVector[1].empty())
        {
            if(getEventCameraId())
            {
                ret[pMasqueradingVector[1]]=(Json::Int64)getValueOfEventCameraId();
            }
            else
            {
                ret[pMasqueradingVector[1]]=Json::Value();
            }
        }
        if(!pMasqueradingVector[2].empty())
        {
            if(getTime())
            {
                ret[pMasqueradingVector[2]]=getTime()->toDbStringLocal();
            }
            else
            {
                ret[pMasqueradingVector[2]]=Json::Value();
            }
        }
        return ret;
    }
    LOG_ERROR << "Masquerade failed";
    if(getTransactionId())
    {
        ret["transaction_id"]=getValueOfTransactionId();
    }
    else
    {
        ret["transaction_id"]=Json::Value();
    }
    if(getEventCameraId())
    {
        ret["event_camera_id"]=(Json::Int64)getValueOfEventCameraId();
    }
    else
    {
        ret["event_camera_id"]=Json::Value();
    }
    if(getTime())
    {
        ret["time"]=getTime()->toDbStringLocal();
    }
    else
    {
        ret["time"]=Json::Value();
    }
    return ret;
}

bool CameraEvent::validateJsonForCreation(const Json::Value &pJson, std::string &err)
{
    if(pJson.isMember("transaction_id"))
    {
        if(!validJsonOfField(0, "transaction_id", pJson["transaction_id"], err, true))
            return false;
    }
    else
    {
        err="The transaction_id column cannot be null";
        return false;
    }
    if(pJson.isMember("event_camera_id"))
    {
        if(!validJsonOfField(1, "event_camera_id", pJson["event_camera_id"], err, true))
            return false;
    }
    if(pJson.isMember("time"))
    {
        if(!validJsonOfField(2, "time", pJson["time"], err, true))
            return false;
    }
    return true;
}
bool CameraEvent::validateMasqueradedJsonForCreation(const Json::Value &pJson,
                                                     const std::vector<std::string> &pMasqueradingVector,
                                                     std::string &err)
{
    if(pMasqueradingVector.size() != 3)
    {
        err = "Bad masquerading vector";
        return false;
    }
    try {
      if(!pMasqueradingVector[0].empty())
      {
          if(pJson.isMember(pMasqueradingVector[0]))
          {
              if(!validJsonOfField(0, pMasqueradingVector[0], pJson[pMasqueradingVector[0]], err, true))
                  return false;
          }
        else
        {
            err="The " + pMasqueradingVector[0] + " column cannot be null";
            return false;
        }
      }
      if(!pMasqueradingVector[1].empty())
      {
          if(pJson.isMember(pMasqueradingVector[1]))
          {
              if(!validJsonOfField(1, pMasqueradingVector[1], pJson[pMasqueradingVector[1]], err, true))
                  return false;
          }
      }
      if(!pMasqueradingVector[2].empty())
      {
          if(pJson.isMember(pMasqueradingVector[2]))
          {
              if(!validJsonOfField(2, pMasqueradingVector[2], pJson[pMasqueradingVector[2]], err, true))
                  return false;
          }
      }
    }
    catch(const Json::LogicError &e)
    {
      err = e.what();
      return false;
    }
    return true;
}
bool CameraEvent::validateJsonForUpdate(const Json::Value &pJson, std::string &err)
{
    if(pJson.isMember("transaction_id"))
    {
        if(!validJsonOfField(0, "transaction_id", pJson["transaction_id"], err, false))
            return false;
    }
    else
    {
        err = "The value of primary key must be set in the json object for update";
        return false;
    }
    if(pJson.isMember("event_camera_id"))
    {
        if(!validJsonOfField(1, "event_camera_id", pJson["event_camera_id"], err, false))
            return false;
    }
    if(pJson.isMember("time"))
    {
        if(!validJsonOfField(2, "time", pJson["time"], err, false))
            return false;
    }
    return true;
}
bool CameraEvent::validateMasqueradedJsonForUpdate(const Json::Value &pJson,
                                                   const std::vector<std::string> &pMasqueradingVector,
                                                   std::string &err)
{
    if(pMasqueradingVector.size() != 3)
    {
        err = "Bad masquerading vector";
        return false;
    }
    try {
      if(!pMasqueradingVector[0].empty() && pJson.isMember(pMasqueradingVector[0]))
      {
          if(!validJsonOfField(0, pMasqueradingVector[0], pJson[pMasqueradingVector[0]], err, false))
              return false;
      }
    else
    {
        err = "The value of primary key must be set in the json object for update";
        return false;
    }
      if(!pMasqueradingVector[1].empty() && pJson.isMember(pMasqueradingVector[1]))
      {
          if(!validJsonOfField(1, pMasqueradingVector[1], pJson[pMasqueradingVector[1]], err, false))
              return false;
      }
      if(!pMasqueradingVector[2].empty() && pJson.isMember(pMasqueradingVector[2]))
      {
          if(!validJsonOfField(2, pMasqueradingVector[2], pJson[pMasqueradingVector[2]], err, false))
              return false;
      }
    }
    catch(const Json::LogicError &e)
    {
      err = e.what();
      return false;
    }
    return true;
}
bool CameraEvent::validJsonOfField(size_t index,
                                   const std::string &fieldName,
                                   const Json::Value &pJson,
                                   std::string &err,
                                   bool isForCreation)
{
    switch(index)
    {
        case 0:
            if(pJson.isNull())
            {
                err="The " + fieldName + " column cannot be null";
                return false;
            }
            if(!pJson.isString())
            {
                err="Type error in the "+fieldName+" field";
                return false;
            }
            break;
        case 1:
            if(pJson.isNull())
            {
                return true;
            }
            if(!pJson.isInt64())
            {
                err="Type error in the "+fieldName+" field";
                return false;
            }
            break;
        case 2:
            if(pJson.isNull())
            {
                return true;
            }
            if(!pJson.isString())
            {
                err="Type error in the "+fieldName+" field";
                return false;
            }
            break;
        default:
            err="Internal error in the server";
            return false;
    }
    return true;
}
