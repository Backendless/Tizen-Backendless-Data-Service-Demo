/*
 * Backendless.h
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

#ifndef BACKENDLESS_H_
#define BACKENDLESS_H_

#include <FBase.h>
#include <FSocial.h>
#include <FNet.h>
#include <FWebJson.h>
#include <FUi.h>

class Backendless
	: public Tizen::Net::Http::IHttpTransactionEventListener
{
public:
	static Backendless* GetInstance();

	void SetExternalProperties(Tizen::Social::Calendarbook* pCalendarbook, Tizen::Base::Collection::IList* pTodosList, Tizen::Ui::Controls::ListView* pListView);
	int GetCurrentRequestEnum();

	result RequestLoadTodosList();
	result RequestSaveTodo(Tizen::Social::CalTodo& calTodo);
	result RequestDeleteTodo(Tizen::Social::CalTodo& calTodo);

protected:
	// IHttpTransactionEventListener
	virtual void OnTransactionReadyToRead(Tizen::Net::Http::HttpSession& httpSession, Tizen::Net::Http::HttpTransaction& httpTransaction, int availableBodyLen);
	virtual void OnTransactionAborted(Tizen::Net::Http::HttpSession& httpSession, Tizen::Net::Http::HttpTransaction& httpTransaction, result r);
	virtual void OnTransactionReadyToWrite(Tizen::Net::Http::HttpSession& httpSession, Tizen::Net::Http::HttpTransaction& httpTransaction, int recommendedChunkSize);
	virtual void OnTransactionHeaderCompleted(Tizen::Net::Http::HttpSession& httpSession, Tizen::Net::Http::HttpTransaction& httpTransaction, int headerLen, bool authRequired);
	virtual void OnTransactionCompleted(Tizen::Net::Http::HttpSession& httpSession, Tizen::Net::Http::HttpTransaction& httpTransaction);
	virtual void OnTransactionCertVerificationRequiredN(Tizen::Net::Http::HttpSession& httpSession, Tizen::Net::Http::HttpTransaction& httpTransaction, Tizen::Base::String* pCert);

private:
	Backendless(void);
	~Backendless(void);

	// HTTP Session
	result CreateHttpSession(void);
	result CreateHttpTransaction(Tizen::Net::Http::NetHttpMethod method, Tizen::Base::String& uri, Tizen::Base::String* entity);

	// JSON Parser
	void SetTodoObjectId(const Tizen::Base::String& objectId);
	Tizen::Social::CalTodo* ParseTodoObject(const Tizen::Web::Json::JsonObject* pJsonObj);
	void ParseJsonBackendlessObject(Tizen::Web::Json::IJsonValue* pJsonValue);
	Tizen::Web::Json::IJsonValue* GetJsonObjectFieldValue(const Tizen::Web::Json::JsonObject* pJsonObj, const Tizen::Base::String& FieldKey, const Tizen::Web::Json::JsonType jsonType);
	Tizen::Web::Json::JsonType GetJsonObjectFieldAsString(const Tizen::Web::Json::JsonObject* pJsonObj, const Tizen::Base::String& FieldKey, Tizen::Base::String* pFieldValue);
	Tizen::Web::Json::JsonType GetJsonObjectFieldAsNumber(const Tizen::Web::Json::JsonObject* pJsonObj, const Tizen::Base::String& FieldKey, Tizen::Web::Json::JsonNumber* pFieldValue);

private:
	// Singlton
	static Backendless* pBackendlessSinglton;

	// Private Properties
	Tizen::Net::Http::HttpSession* __pHttpSession;
	Tizen::Social::CalTodo* __pCalTodo;
	int __OperationRequest;
	int __totalObjects;
	int __pageSize;
	int __offset;

	// External Properties
	Tizen::Social::Calendarbook* __pCalendarbook;
	Tizen::Base::Collection::IList* __pTodosList;
	Tizen::Ui::Controls::ListView* __pListView;
};

#endif /* BACKENDLESS_H_ */
