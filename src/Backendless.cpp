/*
 * Backendless.cpp
 */
/*
 * *********************************************************************************************************************
 *
 *  BACKENDLESS.COM CONFIDENTIAL
 *
 *  ********************************************************************************************************************
 *
 *  Copyright 2014 BACKENDLESS.COM. All Rights Reserved.
 *
 *  NOTICE: All information contained herein is, and remains the property of Backendless.com and its suppliers,
 *  if any. The intellectual and technical concepts contained herein are proprietary to Backendless.com and its
 *  suppliers and may be covered by U.S. and Foreign Patents, patents in process, and are protected by trade secret
 *  or copyright law. Dissemination of this information or reproduction of this material is strictly forbidden
 *  unless prior written permission is obtained from Backendless.com.
 *
 *  ********************************************************************************************************************
 */

#include <new>
#include "Backendless.h"

using namespace Tizen::Base;
using namespace Tizen::Social;
using namespace Tizen::Text;
using namespace Tizen::Net::Http;
using namespace Tizen::Web::Json;
using namespace Tizen::Base::Collection;


const static wchar_t* HTTP_CLIENT_HOST_ADDRESS = L"http://api.backendless.com";
const static wchar_t* BACKENDLESS_APPLICATION_ID = L"****** REPLACE THIS WITH YOUR APP ID ***********";
const static wchar_t* BACKENDLESS_SECRET_KEY = L"****** REPLACE THIS WITH YOUR REST SECREY KEY ***********";

const static wchar_t* JSON_TYPES[] = {L"JSON_TYPE_STRING", L"JSON_TYPE_NUMBER", L"JSON_TYPE_OBJECT", L"JSON_TYPE_ARRAY", L"JSON_TYPE_BOOL", L"JSON_TYPE_NULL"};

enum OperationRequest
{
	OPERATION_REQUEST_NONE,		/**< No operation	*/
	OPERATION_REQUEST_GET_TODOSLIST, /**< Get Todos List	*/
	OPERATION_REQUEST_CREATE_TODO, /**< Create calTodo	*/
	OPERATION_REQUEST_UPDATE_TODO,	/**< Update calTodo	*/
	OPERATION_REQUEST_DELETE_TODO,	/**< Delete calTodo	*/
};


Backendless* Backendless::pBackendlessSinglton = null;

Backendless*
Backendless::GetInstance()
{
	if (!pBackendlessSinglton)
		pBackendlessSinglton = new Backendless();

	return pBackendlessSinglton;
}

Backendless::Backendless(void)
	: __pHttpSession(null)
	, __pCalTodo(null)
	, __OperationRequest(OPERATION_REQUEST_NONE)
	, __totalObjects(0)
	, __pageSize(10)
	, __offset(0)
	, __pCalendarbook(null)
	, __pTodosList(null)
	, __pListView(null)
{
}

Backendless::~Backendless(void)
{
	if (__pHttpSession != null)
	{
		delete __pHttpSession;
		__pHttpSession = null;
	}

	if (__pCalTodo != null)
	{
		delete __pCalTodo;
		__pCalTodo = null;
	}
}

result
Backendless::CreateHttpSession(void)
{
	result r = E_SUCCESS;

	if (__pHttpSession == null)
	{
		//__pHttpSession = new (std::nothrow) HttpSession();
		__pHttpSession = new HttpSession();
		r = __pHttpSession->Construct(NET_HTTP_SESSION_MODE_NORMAL, null, HTTP_CLIENT_HOST_ADDRESS, null);
		if (IsFailed(r)) {
			delete __pHttpSession;
			__pHttpSession = null;
			AppLogException("Backendless:: Failed to create the HttpSession.");
		}
	}

	return r;
}

result
Backendless::CreateHttpTransaction(NetHttpMethod method, String& uri, String* entity)
{
	AppLog("Backendless::CreateHttpTransaction");

	result r = E_SUCCESS;

	HttpTransaction* pHttpTransaction = null;
	HttpRequest* pHttpRequest = null;
	HttpHeader* pHeader = null;
	HttpStringEntity* pHttpStringEntity = null;

	r = CreateHttpSession();
	TryCatch(r == E_SUCCESS, , "Failed to create the HttpSession.");

	r = __pHttpSession->SetAutoRedirectionEnabled(true);
	TryCatch(r == E_SUCCESS, , "Failed to set the redirection automatically.");

	pHttpTransaction = __pHttpSession->OpenTransactionN();
	r = GetLastResult();
	TryCatch(pHttpTransaction != null, , "Failed to open the HttpTransaction.");

	r = pHttpTransaction->AddHttpTransactionListener(*this);
	TryCatch(r == E_SUCCESS, , "Failed to add the HttpTransactionListener.");

	pHttpRequest = const_cast< HttpRequest* >(pHttpTransaction->GetRequest());

	r = pHttpRequest->SetUri(uri);
	TryCatch(r == E_SUCCESS, , "Failed to set the uri.");

	r = pHttpRequest->SetMethod(method);
	TryCatch(r == E_SUCCESS, , "Failed to set the method.");

	// set headers
	pHeader = pHttpRequest->GetHeader();
	pHeader->AddField(L"application-id", BACKENDLESS_APPLICATION_ID);
	pHeader->AddField(L"secret-key", BACKENDLESS_SECRET_KEY);
	pHeader->AddField(L"application-type", L"REST");

	if (entity == null)
	{
		pHeader->AddField(L"Content-Type", L"text/plane");
	}
	else
	{
		//set body
		pHttpStringEntity = new HttpStringEntity();
		Encoding* pEnc = Encoding::GetEncodingN(L"ISO-8859-1");
	    pHttpStringEntity->Construct(*entity, L"application/json", L"ISO-8859-1", *pEnc);
	    r = pHttpRequest->SetEntity(*pHttpStringEntity);
	    delete pHttpStringEntity;
		TryCatch(r == E_SUCCESS, , "Failed to set the body.");
	}

	r = pHttpTransaction->Submit();
	TryCatch(r == E_SUCCESS, , "Failed to submit the HttpTransaction.");

	return r;

CATCH:

	delete pHttpTransaction;
	pHttpTransaction = null;

	AppLog("CreateHttpTransaction() failed. (%s)", GetErrorMessage(r));

	return r;
}

void
Backendless::SetExternalProperties(Tizen::Social::Calendarbook* pCalendarbook, Tizen::Base::Collection::IList* pTodosList, Tizen::Ui::Controls::ListView* pListView)
{
	AppLog("Backendless::SetExternalProperties");

	__pCalendarbook = pCalendarbook;
	__pTodosList = pTodosList;
	__pListView = pListView;
}

int
Backendless::GetCurrentRequestEnum()
{
	return (__OperationRequest == OPERATION_REQUEST_GET_TODOSLIST) ? 1 : 0;
}


result
Backendless::RequestLoadTodosList()
{
	AppLog("Backendless::RequestLoadTodosList");

	if (__pCalendarbook == null || __pTodosList == null)
		return E_FAILURE;

	__OperationRequest = OPERATION_REQUEST_GET_TODOSLIST;
	String uri = L"http://api.backendless.com/v1/data/Todo?offset=";
	uri.Append(__offset);
	return CreateHttpTransaction(NET_HTTP_METHOD_GET, uri, null);
}


result
Backendless::RequestSaveTodo(CalTodo& calTodo)
{
	// create json object
	JsonObject *pJsonObject = new JsonObject();
    pJsonObject->Construct();

	// construct json object
    String *pSubjectKey = new String(L"subject");
	JsonString *pSubject = new JsonString(calTodo.GetSubject());
	pJsonObject->Add(pSubjectKey, pSubject);

	String *pDescriptionKey = new String(L"description");
	JsonString *pDescription = new JsonString(calTodo.GetDescription());
	pJsonObject->Add(pDescriptionKey, pDescription);

	String *pStartDataKey = new String(L"startDate");
	JsonString *pStartData = new JsonString(calTodo.GetStartDate().ToString());
	pJsonObject->Add(pStartDataKey, pStartData);

	String *pDueDataKey = new String(L"dueDate");
	JsonString *pDueData = new JsonString(calTodo.GetDueDate().ToString());
	pJsonObject->Add(pDueDataKey, pDueData);

	String *pPriorityKey = new String(L"priority");
	JsonNumber *pPriority = new JsonNumber((int)calTodo.GetPriority());
	pJsonObject->Add(pPriorityKey, pPriority);

	String *pSensitivityKey = new String(L"sensitivity");
	JsonNumber *pSensitivity = new JsonNumber((int)calTodo.GetSensitivity());
	pJsonObject->Add(pSensitivityKey, pSensitivity);

	String *pStatusKey = new String(L"status");
	JsonNumber *pStatus = new JsonNumber((int)calTodo.GetStatus());
	pJsonObject->Add(pStatusKey, pStatus);

    // compose json object to char*
    char* pComposeBuf = new char[10000];
    result r = JsonWriter::Compose( pJsonObject, pComposeBuf, 10000 );
	if (IsFailed(r))
		*pComposeBuf = 0;
	String json(pComposeBuf);
    delete []pComposeBuf;

    // release json object
    pJsonObject->RemoveAll(true);
    delete pJsonObject;

    AppLog("RequestSaveTodo (%s) LENGTH: %d, [TEXT: %ls]", GetErrorMessage(r), json.GetLength(), json.GetPointer());

    if (IsFailed(r))
		return r;

	if (__pCalTodo != null)
	{
		delete __pCalTodo;
		__pCalTodo = null;
	}

    // send HTTP request
	String uri = L"http://api.backendless.com/v1/data/Todo";
	String objectId = calTodo.GetLocation();
	// CREATE
	if (objectId == null || objectId.GetLength() == 0)
	{
		__OperationRequest = OPERATION_REQUEST_CREATE_TODO;
	    __pCalTodo = new CalTodo(calTodo);
		return CreateHttpTransaction(NET_HTTP_METHOD_POST, uri, &json);
	}

	// UPDATE
	__OperationRequest = OPERATION_REQUEST_UPDATE_TODO;
	uri.Append(L"/"+objectId);
	return CreateHttpTransaction(NET_HTTP_METHOD_PUT, uri, &json);
}

result
Backendless::RequestDeleteTodo(CalTodo& calTodo)
{
	if (__pCalTodo != null)
	{
		delete __pCalTodo;
		__pCalTodo = null;
	}

	String objectId = calTodo.GetLocation();
	if (objectId == null || objectId.GetLength() == 0)
		return  E_FAILURE;

	__OperationRequest = OPERATION_REQUEST_DELETE_TODO;
	String uri = L"http://api.backendless.com/v1/data/Todo/";
	uri.Append(objectId);
	return CreateHttpTransaction(NET_HTTP_METHOD_DELETE, uri, null);
}

void
Backendless::SetTodoObjectId(const String& objectId)
{
	if (__OperationRequest == OPERATION_REQUEST_CREATE_TODO && __pCalTodo != null)
	{
		__pCalTodo->SetLocation(objectId);
		if (__pCalendarbook != null)
		{
			result r = __pCalendarbook->UpdateTodo(*__pCalTodo);
			if (IsFailed(r))
			{
				AppLogException("SetTodoObjectId (%s) Failed to update todo in calendarbook.", GetErrorMessage(r));
			}
		}
		else
		{
			AppLogException("SetTodoObjectId Failed: the calendarbook is not set.");
		}
	}
}

CalTodo*
Backendless::ParseTodoObject(const JsonObject* pJsonObj)
{
	CalTodo* pCalTodo = new CalTodo();
	if (pCalTodo == null)
		return null;

	JsonType jsonType;
	DateTime dateTime;
	result r;

	String objectId = L"";
	jsonType = GetJsonObjectFieldAsString(pJsonObj, L"objectId", &objectId);
	pCalTodo->SetLocation(objectId);
	AppLog("ParseTodoObject (%ls) objectId = %ls", JSON_TYPES[jsonType], objectId.GetPointer());

	String subject = L"";
	jsonType = GetJsonObjectFieldAsString(pJsonObj, L"subject", &subject);
	pCalTodo->SetSubject(subject);
	AppLog("ParseTodoObject (%ls) subject = %ls", JSON_TYPES[jsonType], subject.GetPointer());

	String description = L"";
	jsonType = GetJsonObjectFieldAsString(pJsonObj, L"description", &description);
	pCalTodo->SetDescription(description);
	AppLog("ParseTodoObject (%ls) description = %ls", JSON_TYPES[jsonType], description.GetPointer());

	String startDate = L"";
	jsonType = GetJsonObjectFieldAsString(pJsonObj, L"startDate", &startDate);
	if ( (r = DateTime::Parse(startDate, dateTime)) == E_SUCCESS)
	{
		pCalTodo->SetStartDate(dateTime);
		AppLog("ParseTodoObject (%ls) startDate = %ls", JSON_TYPES[jsonType], startDate.GetPointer());
	}
	else
	{
		AppLogException("ParseTodoObject [ERROR: %s] (%ls) startDate = %ls", GetErrorMessage(r), JSON_TYPES[jsonType], startDate.GetPointer());
	}

	String dueDate = L"";
	jsonType = GetJsonObjectFieldAsString(pJsonObj, L"dueDate", &dueDate);
	if ( (r = DateTime::Parse(dueDate, dateTime)) == E_SUCCESS)
	{
		pCalTodo->SetDueDate(dateTime);
		AppLog("ParseTodoObject (%ls) dueDate = %ls", JSON_TYPES[jsonType], dueDate.GetPointer());
	}
	else
	{
		AppLogException("ParseTodoObject [ERROR: %s] (%ls) dueDate = %ls", GetErrorMessage(r), JSON_TYPES[jsonType], dueDate.GetPointer());
	}

	JsonNumber priority(0);
	jsonType = GetJsonObjectFieldAsNumber(pJsonObj, L"priority", &priority);
	pCalTodo->SetPriority((TodoPriority)priority.ToInt());
	AppLog("ParseTodoObject (%ls) priority = %d", JSON_TYPES[jsonType],  priority.ToInt());

	JsonNumber sensitivity(0);
	jsonType = GetJsonObjectFieldAsNumber(pJsonObj, L"sensitivity", &sensitivity);
	pCalTodo->SetSensitivity((RecordSensitivity)sensitivity.ToInt());
	AppLog("ParseTodoObject (%ls) sensitivity = %d", JSON_TYPES[jsonType],  sensitivity.ToInt());

	JsonNumber status(0);
	jsonType = GetJsonObjectFieldAsNumber(pJsonObj, L"status", &status);
	pCalTodo->SetStatus((TodoStatus)status.ToInt());
	AppLog("ParseTodoObject (%ls) status = %d", JSON_TYPES[jsonType],  status.ToInt());

	if (__OperationRequest == OPERATION_REQUEST_GET_TODOSLIST && __pCalendarbook != null && __pTodosList != null)
	{
		r = __pCalendarbook->AddTodo(*pCalTodo);
		if (IsFailed(r))
		{
			AppLogException("ParseTodoObject Calendarbook->AddTodo failed. (%s)", GetErrorMessage(r));
		}
		else
		{
			__pTodosList->Add(pCalTodo);
		}
	}

	return pCalTodo;
}

void
Backendless::ParseJsonBackendlessObject(IJsonValue* pJsonValue)
{
	if ( pJsonValue == null )
	{
		AppLogException("ParseJsonBackendlessObject (FAULT) pJsonValue: <%s>", GetLastResult());
		return;
	}

	// Uses the pJsonValue to know what is the type
	JsonType jsonType = pJsonValue->GetType();
	switch (jsonType) {

		case JSON_TYPE_OBJECT:
		{
			JsonObject* pJsonObj = static_cast<JsonObject*>(pJsonValue); //use pJsonObj to access key-value pairs

			// get 'objectId' field (must be in backendless object)
			String objectId = L"";
			if ( (jsonType = GetJsonObjectFieldAsString(pJsonObj, L"objectId", &objectId)) != JSON_TYPE_STRING)
			{
				AppLog("ParseJsonBackendlessObject (OPTION) <NOT BACKENDLESS OBJECT - MAY BE A BACKENDLESS COLLECTION?>");

				IJsonValue* pValue = null;
				if ( (pValue = GetJsonObjectFieldValue(pJsonObj, L"data", JSON_TYPE_ARRAY)) != null )
				{
					JsonArray* pJsonArray = static_cast<JsonArray*>(pValue); //use pJsonArray to access values

					JsonNumber totalObjects(0);
					jsonType = GetJsonObjectFieldAsNumber(pJsonObj, L"totalObjects", &totalObjects);
					__totalObjects = (jsonType == JSON_TYPE_NUMBER) ? totalObjects.ToInt() : pJsonArray->GetCount();

					JsonNumber offset(0);
					jsonType = GetJsonObjectFieldAsNumber(pJsonObj, L"offset", &offset);
					__offset = (jsonType == JSON_TYPE_NUMBER) ? offset.ToInt() : -1;

					AppLog("ParseJsonBackendlessObject (SUCCESS) COLLECTION count = %d [totalObjects = %d, pageSize = %d, offset = %d]", pJsonArray->GetCount(), __totalObjects, __pageSize, __offset);

			        // Uses enumerator to access elements in the JsonArray
			        IEnumeratorT<IJsonValue*>* pEnum = pJsonArray->GetEnumeratorN();
			        if(pEnum)
			        {
		                IJsonValue* pNextValue = null;
			            while( pEnum->MoveNext() == E_SUCCESS )
			            {
			                pEnum->GetCurrent(pNextValue);
			                ParseJsonBackendlessObject(pNextValue);
			            }

			            delete pEnum;
			        }

			        AppLog("ParseJsonBackendlessObject (SUCCESS) TODOS LIST count = %d", __pTodosList->GetCount());

			        pJsonArray->RemoveAll(true);
				}
			}
			else
			{
				AppLog("ParseJsonBackendlessObject (SUCCESS) objectId = %ls", objectId.GetPointer());

				// save objectId in __pCalTodo.Location
				SetTodoObjectId(objectId);

				// construct calTodo
				ParseTodoObject(pJsonObj);
			}

			pJsonObj->RemoveAll(true);

			break;
		}

		default:
		{
			AppLogException("ParseJsonBackendlessObject <UNPROCESSED TYPE %ls>", JSON_TYPES[jsonType]);
			break;
		}
	}
}

IJsonValue*
Backendless::GetJsonObjectFieldValue(const JsonObject* pJsonObj, const String& FieldKey, const JsonType jsonType)
{

	IJsonValue* pValue = null;
	pJsonObj->GetValue(&FieldKey, pValue);
	if (pValue == null || pValue->GetType() != jsonType)
	{
		AppLogException("GetJsonObjectFieldValue (ERROR) <FIELD '%ls' OF TYPE %ls IS ABSENT>", FieldKey.GetPointer(), JSON_TYPES[jsonType]);
		return null;
	}

	return pValue;
}

JsonType
Backendless::GetJsonObjectFieldAsString(const JsonObject* pJsonObj, const String& FieldKey, String* pFieldValue)
{
	if (pFieldValue == null)
	{
		return JSON_TYPE_NULL;
	}

	IJsonValue* pValue = null;
	pJsonObj->GetValue(&FieldKey, pValue);
	if (pValue == null)
	{
		return JSON_TYPE_NULL;
	}

	JsonType jsonType = pValue->GetType();
	if (jsonType != JSON_TYPE_STRING)
	{
		return jsonType;
	}

	JsonString* pString = static_cast<JsonString*>(pValue);
	String str(pString->GetPointer());
	*pFieldValue = str;

	return JSON_TYPE_STRING;
}

JsonType
Backendless::GetJsonObjectFieldAsNumber(const JsonObject* pJsonObj, const String& FieldKey, JsonNumber* pFieldValue)
{
	if (pFieldValue == null)
	{
		return JSON_TYPE_NULL;
	}

	IJsonValue* pValue = null;
	pJsonObj->GetValue(&FieldKey, pValue);
	if (pValue == null)
	{
		return JSON_TYPE_NULL;
	}

	JsonType jsonType = pValue->GetType();
	if (jsonType != JSON_TYPE_NUMBER)
	{
		return jsonType;
	}

	JsonNumber* pNumber = static_cast<JsonNumber*>(pValue);
	*pFieldValue = *pNumber;

	return JSON_TYPE_NUMBER;
}

void
Backendless::OnTransactionReadyToRead(HttpSession& httpSession, HttpTransaction& httpTransaction, int availableBodyLen)
{

	HttpResponse* pHttpResponse = httpTransaction.GetResponse();
	int status = pHttpResponse->GetHttpStatusCode();

	if (status == HTTP_STATUS_OK)
	{
		HttpHeader* pHttpHeader = pHttpResponse->GetHeader();
		if (pHttpHeader != null)
		{
			String* tempHeaderString = pHttpHeader->GetRawHeaderN();
			ByteBuffer* pBuffer = pHttpResponse->ReadBodyN();

			// swow the response
			String text((char *)(pBuffer->GetPointer()));
			int tail = text.GetLength() - availableBodyLen;
			if (tail > 0) {
				text.Remove(availableBodyLen, tail);
			}

			AppLog("OnTransactionReadyToRead STATUS: %d LENGTH: %d, TEXT: '%ls'", status, text.GetLength(), text.GetPointer());

			text = *tempHeaderString + text;

			// parse json response
			IJsonValue* pJsonValue = JsonParser::ParseN(*pBuffer);
			ParseJsonBackendlessObject(pJsonValue);
			delete pJsonValue;

			delete tempHeaderString;
			delete pBuffer;
		}
		else
		{
			AppLogException("OnTransactionReadyToRead STATUS: %d [ERROR: No Headers]", status);
		}
	}
	else
	{
		AppLogException("OnTransactionReadyToRead [ERROR] STATUS: %d", status);
	}
}

void
Backendless::OnTransactionAborted(HttpSession& httpSession, HttpTransaction& httpTransaction, result r)
{
	AppLogException("OnTransactionAborted(%s)", GetErrorMessage(r));

	delete &httpTransaction;
}

void
Backendless::OnTransactionReadyToWrite(HttpSession& httpSession, HttpTransaction& httpTransaction, int recommendedChunkSize)
{
	AppLog("OnTransactionReadyToWrite");
}

void
Backendless::OnTransactionHeaderCompleted(HttpSession& httpSession, HttpTransaction& httpTransaction, int headerLen, bool authRequired)
{
	AppLog("OnTransactionHeaderCompleted");
}

void
Backendless::OnTransactionCompleted(HttpSession& httpSession, HttpTransaction& httpTransaction)
{
	AppLog("OnTransactionCompleted");

	delete &httpTransaction;

	if (__OperationRequest == OPERATION_REQUEST_GET_TODOSLIST && __pTodosList != null && __pListView != null)
	{
		int sizeList = __pTodosList->GetCount();

		AppLog("OnTransactionCompleted: LIST SIZE = %d [totalObjects = %d, offset = %d]", sizeList, __totalObjects, __offset);

		if (sizeList < __totalObjects)
		{
			__offset = sizeList;
			RequestLoadTodosList();
		}
		else
		{
			__offset = 0;
			__OperationRequest = OPERATION_REQUEST_NONE;
			__pListView->UpdateList();
		}
	}
	else
	{
		__OperationRequest = OPERATION_REQUEST_NONE;
	}
}

void
Backendless::OnTransactionCertVerificationRequiredN(HttpSession& httpSession, HttpTransaction& httpTransaction, Tizen::Base::String* pCert)
{
	AppLog("OnTransactionCertVerificationRequiredN");

	httpTransaction.Resume();

	delete pCert;
}


