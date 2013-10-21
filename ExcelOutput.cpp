#include "ExcelOutput.h"
#include "Tranche.h"
#include "MtgCashFlow.h"
#include "StressTest.h"
HRESULT ExcelOutput::PrintMortgagesRepLines(
	const MtgCashFlow& source,
	const QString& DestinationAddress,
	bool PrintCounter, 
	bool PrintDates, 
	bool PrintOutstanding, 
	bool PrintInterest, 
	bool PrintScheduled, 
	bool PrintPrepay, 
	bool PrintAccruedInterest, 
	bool PrintTotalPrincipal,
	bool PrintLoss,
	bool PrintLossOnInterest
	){
		ExcelCommons::InitExcelOLE();
		int NumOfCols=
			PrintCounter+PrintDates+PrintOutstanding+PrintInterest+PrintScheduled+PrintPrepay+PrintAccruedInterest+PrintTotalPrincipal+PrintTotalPrincipal+PrintLoss+PrintLossOnInterest;
		int Numrows=source.Count();
		SAFEARRAYBOUND  Bound[2];
		Bound[0].lLbound   = 1;
		Bound[0].cElements = Numrows;
		Bound[1].lLbound   = 0;
		Bound[1].cElements = NumOfCols;
		SAFEARRAY* saData;
		saData = SafeArrayCreate(VT_VARIANT, 2, Bound);
		VARIANT HUGEP *pdFreq;
		HRESULT hr = SafeArrayAccessData(saData, (void HUGEP* FAR*)&pdFreq);
		if (SUCCEEDED(hr))
		{
			if(PrintCounter){
				for (int i = 0; i < Numrows; i++)
				{
					pdFreq->vt = VT_I4;
					pdFreq->intVal = i;
					pdFreq++;
				}
			}
			if(PrintDates){
				for (int i = 0; i < Numrows; i++)
				{
					pdFreq->vt = VT_BSTR;
					pdFreq->bstrVal = SysAllocString(source.GetDate(i).toString("yyyy-MM-dd").toStdWString().c_str());
					pdFreq++;
				}
			}
			if(PrintOutstanding){
				for (int i = 0; i < Numrows; i++)
				{
					pdFreq->vt = VT_R8;
					pdFreq->dblVal = source.GetAmountOut(i);
					pdFreq++;
				}
			}
			if(PrintInterest){
				for (int i = 0; i < Numrows; i++)
				{
					pdFreq->vt = VT_R8;
					pdFreq->dblVal = source.GetInterest(i);
					pdFreq++;
				}
			}
			if(PrintScheduled){
				for (int i = 0; i < Numrows; i++)
				{
					pdFreq->vt = VT_R8;
					pdFreq->dblVal = source.GetScheduled(i);
					pdFreq++;
				}
			}
			if(PrintPrepay){
				for (int i = 0; i < Numrows; i++)
				{
					pdFreq->vt = VT_R8;
					pdFreq->dblVal = source.GetPrepay(i);
					pdFreq++;
				}
			}
			if(PrintAccruedInterest){
				for (int i = 0; i < Numrows; i++)
				{
					pdFreq->vt = VT_R8;
					pdFreq->dblVal = source.GetAccruedInterest(i);
					pdFreq++;
				}
			}
			if(PrintTotalPrincipal){
				for (int i = 0; i < Numrows; i++)
				{
					pdFreq->vt = VT_R8;
					pdFreq->dblVal = source.GetPrincipal(i);
					pdFreq++;
				}
			}
			if(PrintLoss){
				for (int i = 0; i < Numrows; i++)
				{
					pdFreq->vt = VT_R8;
					pdFreq->dblVal = source.GetLoss(i);
					pdFreq++;
				}
			}
			if(PrintLossOnInterest){
				for (int i = 0; i < Numrows; i++)
				{
					pdFreq->vt = VT_R8;
					pdFreq->dblVal = source.GetLossOnInterest(i);
					pdFreq++;
				}
			}
			SafeArrayUnaccessData(saData);
		}
		static DISPID dispid = 0;
		DISPPARAMS Params;
		VARIANTARG Command[13];
		int CurrentCmdIndex=13-1;
		if(!ExcelCommons::pExcelDisp)return S_FALSE;
		try
		{
			Command[CurrentCmdIndex].vt = VT_BSTR;
			Command[CurrentCmdIndex--].bstrVal = SysAllocString(L"PrintMortgagesRepLines");
			Command[CurrentCmdIndex].vt = VT_ARRAY | VT_VARIANT;
			Command[CurrentCmdIndex--].parray = saData;
			Command[CurrentCmdIndex].vt = VT_BSTR;
			Command[CurrentCmdIndex--].bstrVal = SysAllocString(DestinationAddress.toStdWString().c_str());
			Command[CurrentCmdIndex].vt = VT_BOOL;
			Command[CurrentCmdIndex--].boolVal=PrintCounter;
			Command[CurrentCmdIndex].vt = VT_BOOL;
			Command[CurrentCmdIndex--].boolVal=PrintDates;
			Command[CurrentCmdIndex].vt = VT_BOOL;
			Command[CurrentCmdIndex--].boolVal=PrintOutstanding;
			Command[CurrentCmdIndex].vt = VT_BOOL;
			Command[CurrentCmdIndex--].boolVal=PrintInterest;
			Command[CurrentCmdIndex].vt = VT_BOOL;
			Command[CurrentCmdIndex--].boolVal=PrintScheduled;
			Command[CurrentCmdIndex].vt = VT_BOOL;
			Command[CurrentCmdIndex--].boolVal=PrintPrepay;
			Command[CurrentCmdIndex].vt = VT_BOOL;
			Command[CurrentCmdIndex--].boolVal=PrintAccruedInterest;
			Command[CurrentCmdIndex].vt = VT_BOOL;
			Command[CurrentCmdIndex--].boolVal=PrintTotalPrincipal;
			Command[CurrentCmdIndex].vt = VT_BOOL;
			Command[CurrentCmdIndex--].boolVal=PrintLoss;
			Command[CurrentCmdIndex].vt = VT_BOOL;
			Command[CurrentCmdIndex--].boolVal=PrintLossOnInterest;

			Params.rgdispidNamedArgs = NULL;
			Params.rgvarg=Command;
			Params.cArgs = 13;
			Params.cNamedArgs = 0;
			if(dispid == 0)
			{
				wchar_t *ucName = L"Run";
				hr = ExcelCommons::pExcelDisp->GetIDsOfNames(IID_NULL, &ucName, 1,
					LOCALE_SYSTEM_DEFAULT, &dispid);
				if(FAILED(hr))
				{
					SysFreeString(Params.rgvarg[Params.cArgs-1].bstrVal);
					SysFreeString(Params.rgvarg[Params.cArgs-3].bstrVal);
					if(PrintDates){
						SafeArrayAccessData(saData, (void HUGEP* FAR*)&pdFreq);
						if(PrintCounter) pdFreq+=Numrows;
						for (DWORD i = 0; i < Numrows; i++)
						{
							SysFreeString(pdFreq->bstrVal);
							pdFreq++;
						}
						SafeArrayUnaccessData(saData);
					}
					return hr;
				}
			}
			hr = ExcelCommons::pExcelDisp->Invoke(dispid,IID_NULL,LOCALE_SYSTEM_DEFAULT,
				DISPATCH_METHOD, &Params, NULL, NULL, NULL);
			if(FAILED(hr))
			{
				SysFreeString(Params.rgvarg[Params.cArgs-1].bstrVal);
				SysFreeString(Params.rgvarg[Params.cArgs-3].bstrVal);
				if(PrintDates){
					SafeArrayAccessData(saData, (void HUGEP* FAR*)&pdFreq);
					if(PrintCounter) pdFreq+=Numrows;
					for (DWORD i = 0; i < Numrows; i++)
					{
						SysFreeString(pdFreq->bstrVal);
						pdFreq++;
					}
					SafeArrayUnaccessData(saData);
				}
				return hr;
			}
		}
		catch(_com_error &ce)
		{
			hr = ce.Error();
		}
		SysFreeString(Params.rgvarg[Params.cArgs-1].bstrVal);
		SysFreeString(Params.rgvarg[Params.cArgs-3].bstrVal);
		if(PrintDates){
			SafeArrayAccessData(saData, (void HUGEP* FAR*)&pdFreq);
			if(PrintCounter) pdFreq+=Numrows;
			for (DWORD i = 0; i < Numrows; i++)
			{
				SysFreeString(pdFreq->bstrVal);
				pdFreq++;
			}
			SafeArrayUnaccessData(saData);
		}
		return hr;
}

HRESULT ExcelOutput::PrintTrancheFlow(
	const Tranche& source,
	const QString& DestinationAddress,
	const QColor& BackgrndCol,
	bool PrintDates, 
	bool PrintOutstanding, 
	bool PrintInterest, 
	bool PrintPrincipal, 
	bool PrintTotalFlow, 
	bool PrintDeferred, 
	bool PrintOCtest, 
	bool PrintICtest
	){
		ExcelCommons::InitExcelOLE();
		QString ColorString=QString("%1,%2,%3").arg(BackgrndCol.red()).arg(BackgrndCol.green()).arg(BackgrndCol.blue());
		int NumOfCols=
			PrintDates+PrintOutstanding+PrintInterest+PrintPrincipal+PrintTotalFlow+PrintDeferred+PrintOCtest+PrintICtest;
		int Numrows= source.GetCashFlow().Count();
		SAFEARRAYBOUND  Bound[2];
		Bound[0].lLbound   = 1;
		Bound[0].cElements = Numrows;
		Bound[1].lLbound   = 0;
		Bound[1].cElements = NumOfCols;
		SAFEARRAY* saData;
		saData = SafeArrayCreate(VT_VARIANT, 2, Bound);
		VARIANT HUGEP *pdFreq;
		HRESULT hr = SafeArrayAccessData(saData, (void HUGEP* FAR*)&pdFreq);
		if (SUCCEEDED(hr))
		{
			if(PrintDates){
				for (DWORD i = 0; i < Numrows; i++)
				{
					pdFreq->vt = VT_BSTR;
					pdFreq->bstrVal = SysAllocString(source.GetCashFlow().GetDate(i).toString("yyyy-MM-dd").toStdWString().c_str());
					pdFreq++;
				}
			}
			if(PrintOutstanding){
				for (DWORD i = 0; i < Numrows; i++)
				{
					pdFreq->vt = VT_R8;
					pdFreq->dblVal = source.GetCashFlow().GetAmountOutstanding(i);
					pdFreq++;
				}
			}
			if(PrintInterest){
				for (DWORD i = 0; i < Numrows; i++)
				{
					pdFreq->vt = VT_R8;
					pdFreq->dblVal = source.GetCashFlow().GetInterest(i);
					pdFreq++;
				}
			}
			if(PrintPrincipal){
				for (DWORD i = 0; i < Numrows; i++)
				{
					pdFreq->vt = VT_R8;
					pdFreq->dblVal = source.GetCashFlow().GetPrincipal(i);
					pdFreq++;
				}
			}
			if(PrintTotalFlow){
				for (DWORD i = 0; i < Numrows; i++)
				{
					pdFreq->vt = VT_R8;
					pdFreq->dblVal = source.GetCashFlow().GetTotalFlow(i);
					pdFreq++;
				}
			}
			if(PrintDeferred){
				for (DWORD i = 0; i < Numrows; i++)
				{
					pdFreq->vt = VT_R8;
					pdFreq->dblVal = source.GetCashFlow().GetDeferred(i);
					pdFreq++;
				}
			}
			if(PrintOCtest){
				for (DWORD i = 0; i < Numrows; i++)
				{
					pdFreq->vt = VT_R8;
					pdFreq->dblVal = source.GetCashFlow().GetOCTest(i);
					pdFreq++;
				}
			}
			if(PrintICtest){
				for (DWORD i = 0; i < Numrows; i++)
				{
					pdFreq->vt = VT_R8;
					pdFreq->dblVal = source.GetCashFlow().GetICTest(i);
					pdFreq++;
				}
			}
			SafeArrayUnaccessData(saData);
		}




		static DISPID dispid = 0;
		DISPPARAMS Params;
		VARIANTARG Command[15];
		int CurrentCmdIndex=15-1;
		if(!ExcelCommons::pExcelDisp)return S_FALSE;
		try
		{
			Command[CurrentCmdIndex].vt = VT_BSTR;
			Command[CurrentCmdIndex--].bstrVal = SysAllocString(L"PrintTranche");
			Command[CurrentCmdIndex].vt = VT_ARRAY | VT_VARIANT;
			Command[CurrentCmdIndex--].parray = saData;
			Command[CurrentCmdIndex].vt = VT_BSTR;
			Command[CurrentCmdIndex--].bstrVal = SysAllocString(DestinationAddress.toStdWString().c_str());
			Command[CurrentCmdIndex].vt = VT_BSTR;
			Command[CurrentCmdIndex--].bstrVal = SysAllocString(source.GetTrancheName().toStdWString().c_str());
			Command[CurrentCmdIndex].vt = VT_BSTR;
			Command[CurrentCmdIndex--].bstrVal = SysAllocString(ColorString.toStdWString().c_str());
			Command[CurrentCmdIndex].vt = VT_BOOL;
			Command[CurrentCmdIndex--].boolVal=PrintDates;
			Command[CurrentCmdIndex].vt = VT_BOOL;
			Command[CurrentCmdIndex--].boolVal=PrintOutstanding;
			Command[CurrentCmdIndex].vt = VT_BOOL;
			Command[CurrentCmdIndex--].boolVal=PrintInterest;
			Command[CurrentCmdIndex].vt = VT_BOOL;
			Command[CurrentCmdIndex--].boolVal=PrintPrincipal;
			Command[CurrentCmdIndex].vt = VT_BOOL;
			Command[CurrentCmdIndex--].boolVal=PrintTotalFlow;
			Command[CurrentCmdIndex].vt = VT_BOOL;
			Command[CurrentCmdIndex--].boolVal=PrintDeferred;
			Command[CurrentCmdIndex].vt = VT_BOOL;
			Command[CurrentCmdIndex--].boolVal=PrintOCtest;
			Command[CurrentCmdIndex].vt = VT_BOOL;
			Command[CurrentCmdIndex--].boolVal=PrintICtest;
			Command[CurrentCmdIndex].vt = VT_R8;
			Command[CurrentCmdIndex--].dblVal=source.GetMinIClevel();
			Command[CurrentCmdIndex].vt = VT_R8;
			Command[CurrentCmdIndex--].dblVal=source.GetMinOClevel();

			
			Params.rgdispidNamedArgs = NULL;
			Params.rgvarg=Command;
			Params.cArgs = 15;
			Params.cNamedArgs = 0;
			if(dispid == 0)
			{
				wchar_t *ucName = L"Run";
				hr = ExcelCommons::pExcelDisp->GetIDsOfNames(IID_NULL, &ucName, 1,
					LOCALE_SYSTEM_DEFAULT, &dispid);
				if(FAILED(hr))
				{
					SysFreeString(Params.rgvarg[Params.cArgs-1].bstrVal);
					SysFreeString(Params.rgvarg[Params.cArgs-3].bstrVal);
					SysFreeString(Params.rgvarg[Params.cArgs-4].bstrVal);
					SysFreeString(Params.rgvarg[Params.cArgs-5].bstrVal);
					if(PrintDates){
						SafeArrayAccessData(saData, (void HUGEP* FAR*)&pdFreq);
						for (DWORD i = 0; i < Numrows; i++)
						{
							SysFreeString(pdFreq->bstrVal);
							pdFreq++;
						}
						SafeArrayUnaccessData(saData);
					}
					return hr;
				}
			}
			hr = ExcelCommons::pExcelDisp->Invoke(dispid,IID_NULL,LOCALE_SYSTEM_DEFAULT,
				DISPATCH_METHOD, &Params, NULL, NULL, NULL);
			if(FAILED(hr))
			{
				SysFreeString(Params.rgvarg[Params.cArgs-1].bstrVal);
				SysFreeString(Params.rgvarg[Params.cArgs-3].bstrVal);
				SysFreeString(Params.rgvarg[Params.cArgs-4].bstrVal);
				SysFreeString(Params.rgvarg[Params.cArgs-5].bstrVal);
				if(PrintDates){
					SafeArrayAccessData(saData, (void HUGEP* FAR*)&pdFreq);
					for (DWORD i = 0; i < Numrows; i++)
					{
						SysFreeString(pdFreq->bstrVal);
						pdFreq++;
					}
					SafeArrayUnaccessData(saData);
				}
				return hr;
			}
		}
		catch(_com_error &ce)
		{
			hr = ce.Error();
		}
		SysFreeString(Params.rgvarg[Params.cArgs-1].bstrVal);
		SysFreeString(Params.rgvarg[Params.cArgs-3].bstrVal);
		SysFreeString(Params.rgvarg[Params.cArgs-4].bstrVal);
		SysFreeString(Params.rgvarg[Params.cArgs-5].bstrVal);
		if(PrintDates){
			SafeArrayAccessData(saData, (void HUGEP* FAR*)&pdFreq);
			for (DWORD i = 0; i < Numrows; i++)
			{
				SysFreeString(pdFreq->bstrVal);
				pdFreq++;
			}
			SafeArrayUnaccessData(saData);
		}
		return hr;
}


HRESULT ExcelOutput::PrintStressTest(const StressTest& stresser, const QString& TrancheTarget, const QString& DestinationAddress, bool SetupConditionalFormatting){
	ExcelCommons::InitExcelOLE();
	int Numrows=stresser.GetXSpann().size();
	int NumOfCols=stresser.GetYSpann().size();
	SAFEARRAYBOUND  Bound[2];
	Bound[0].lLbound   = 1;
	Bound[0].cElements = Numrows;
	Bound[1].lLbound   = 0;
	Bound[1].cElements = NumOfCols;
	SAFEARRAY* saData;
	saData = SafeArrayCreate(VT_VARIANT, 2, Bound);
	VARIANT HUGEP *pdFreq;
	HRESULT hr = SafeArrayAccessData(saData, (void HUGEP* FAR*)&pdFreq);
	if (SUCCEEDED(hr))
	{
		for(int i=0;i<NumOfCols;i++){
			for(int j=0;j<Numrows;j++){
				pdFreq->vt = VT_R8;
				pdFreq->dblVal = stresser.GetResults().value(stresser.GetXSpann().at(j)).value(stresser.GetYSpann().at(i)).GetTranche(TrancheTarget)->GetLossRate();
				pdFreq++;
			}
		}
		SafeArrayUnaccessData(saData);
	}
	static DISPID dispid = 0;
	DISPPARAMS Params;
	VARIANTARG Command[4];
	int CurrentCmdIndex=4-1;
	if(!ExcelCommons::pExcelDisp)return S_FALSE;
	try
	{
		Command[CurrentCmdIndex].vt = VT_BSTR;
		Command[CurrentCmdIndex--].bstrVal = SysAllocString(L"PrintStressTest");
		Command[CurrentCmdIndex].vt = VT_ARRAY | VT_VARIANT;
		Command[CurrentCmdIndex--].parray = saData;
		Command[CurrentCmdIndex].vt = VT_BSTR;
		Command[CurrentCmdIndex--].bstrVal = SysAllocString(ExcelCommons::CellOffset(DestinationAddress,0,0).toStdWString().c_str());
		Command[CurrentCmdIndex].vt = VT_BOOL;
		Command[CurrentCmdIndex--].boolVal=SetupConditionalFormatting;
		Params.rgdispidNamedArgs = NULL;
		Params.rgvarg=Command;
		Params.cArgs = 4;
		Params.cNamedArgs = 0;
		if(dispid == 0)
		{
			wchar_t *ucName = L"Run";
			hr = ExcelCommons::pExcelDisp->GetIDsOfNames(IID_NULL, &ucName, 1,
				LOCALE_SYSTEM_DEFAULT, &dispid);
			if(FAILED(hr))
			{
				SysFreeString(Params.rgvarg[Params.cArgs-1].bstrVal);
				SysFreeString(Params.rgvarg[Params.cArgs-3].bstrVal);
				return hr;
			}
		}
		hr = ExcelCommons::pExcelDisp->Invoke(dispid,IID_NULL,LOCALE_SYSTEM_DEFAULT,
			DISPATCH_METHOD, &Params, NULL, NULL, NULL);
		if(FAILED(hr))
		{
			SysFreeString(Params.rgvarg[Params.cArgs-1].bstrVal);
			SysFreeString(Params.rgvarg[Params.cArgs-3].bstrVal);
			return hr;
		}
	}
	catch(_com_error &ce)
	{
		hr = ce.Error();
	}
	SysFreeString(Params.rgvarg[Params.cArgs-1].bstrVal);
	SysFreeString(Params.rgvarg[Params.cArgs-3].bstrVal);
	return hr;
}
HRESULT ExcelOutput::PrintMergedCell(const QString& msg, const QString& TargetCell, int RowDim, int ColDim,const QColor& FillColor){
	ExcelCommons::InitExcelOLE();
	QString ColorString=
		QString("%1,%2,%3").arg(FillColor.red()).arg(FillColor.green()).arg(FillColor.blue());
	HRESULT hr;
	static DISPID dispid = 0;
	DISPPARAMS Params;
	VARIANTARG Command[6];
	int CurrentCmdIndex=6-1;
	if(!ExcelCommons::pExcelDisp)return S_FALSE;
	try
	{
		Command[CurrentCmdIndex].vt = VT_BSTR;
		Command[CurrentCmdIndex--].bstrVal = SysAllocString(L"PrintMergedCell");
		Command[CurrentCmdIndex].vt = VT_BSTR;
		Command[CurrentCmdIndex--].bstrVal = SysAllocString(msg.toStdWString().c_str());
		Command[CurrentCmdIndex].vt = VT_BSTR;
		Command[CurrentCmdIndex--].bstrVal = SysAllocString(TargetCell.toStdWString().c_str());
		Command[CurrentCmdIndex].vt = VT_I4;
		Command[CurrentCmdIndex--].intVal=RowDim;
		Command[CurrentCmdIndex].vt = VT_I4;
		Command[CurrentCmdIndex--].intVal=ColDim;
		Command[CurrentCmdIndex].vt = VT_BSTR;
		Command[CurrentCmdIndex--].bstrVal = SysAllocString(ColorString.toStdWString().c_str());
		Params.rgdispidNamedArgs = NULL;
		Params.rgvarg=Command;
		Params.cArgs = 6;
		Params.cNamedArgs = 0;
		if(dispid == 0)
		{
			wchar_t *ucName = L"Run";
			hr = ExcelCommons::pExcelDisp->GetIDsOfNames(IID_NULL, &ucName, 1,
				LOCALE_SYSTEM_DEFAULT, &dispid);
			if(FAILED(hr))
			{
				SysFreeString(Params.rgvarg[Params.cArgs-1].bstrVal);
				SysFreeString(Params.rgvarg[Params.cArgs-2].bstrVal);
				SysFreeString(Params.rgvarg[Params.cArgs-3].bstrVal);
				SysFreeString(Params.rgvarg[Params.cArgs-6].bstrVal);
				return hr;
			}
		}
		hr = ExcelCommons::pExcelDisp->Invoke(dispid,IID_NULL,LOCALE_SYSTEM_DEFAULT,
			DISPATCH_METHOD, &Params, NULL, NULL, NULL);
		if(FAILED(hr))
		{
			SysFreeString(Params.rgvarg[Params.cArgs-1].bstrVal);
			SysFreeString(Params.rgvarg[Params.cArgs-2].bstrVal);
			SysFreeString(Params.rgvarg[Params.cArgs-3].bstrVal);
			SysFreeString(Params.rgvarg[Params.cArgs-6].bstrVal);
			return hr;
		}
	}
	catch(_com_error &ce)
	{
		hr = ce.Error();
	}
	SysFreeString(Params.rgvarg[Params.cArgs-1].bstrVal);
	SysFreeString(Params.rgvarg[Params.cArgs-2].bstrVal);
	SysFreeString(Params.rgvarg[Params.cArgs-3].bstrVal);
	SysFreeString(Params.rgvarg[Params.cArgs-6].bstrVal);
	return hr;
}
HRESULT ExcelOutput::PrintColumn(const QString& Title, const QList<double>& Values ,const QString& TargetCell,const QString& ValFormat,const QColor& FillColor){
	ExcelCommons::InitExcelOLE();
	QString ColorString=
		QString("%1,%2,%3").arg(FillColor.red()).arg(FillColor.green()).arg(FillColor.blue());
	SAFEARRAYBOUND  Bound;
	Bound.lLbound   = 1;
	Bound.cElements = Values.size();
	SAFEARRAY* saData;
	saData = SafeArrayCreate(VT_VARIANT, 1, &Bound);
	VARIANT HUGEP *pdFreq;
	HRESULT hr = SafeArrayAccessData(saData, (void HUGEP* FAR*)&pdFreq);
	if (SUCCEEDED(hr))
	{
		foreach(const double& singleValue,Values){
			pdFreq->vt = VT_R8;
			pdFreq->dblVal = singleValue;
			pdFreq++;
		}
		SafeArrayUnaccessData(saData);
	}
	static DISPID dispid = 0;
	DISPPARAMS Params;
	VARIANTARG Command[6];
	int CurrentCmdIndex=6-1;
	if(!ExcelCommons::pExcelDisp)return S_FALSE;
	try
	{
		Command[CurrentCmdIndex].vt = VT_BSTR;
		Command[CurrentCmdIndex--].bstrVal = SysAllocString(L"PrintColumn");
		Command[CurrentCmdIndex].vt = VT_BSTR;
		Command[CurrentCmdIndex--].bstrVal = SysAllocString(Title.toStdWString().c_str());
		Command[CurrentCmdIndex].vt = VT_ARRAY | VT_VARIANT;
		Command[CurrentCmdIndex--].parray = saData;
		Command[CurrentCmdIndex].vt = VT_BSTR;
		Command[CurrentCmdIndex--].bstrVal = SysAllocString(TargetCell.toStdWString().c_str());
		Command[CurrentCmdIndex].vt = VT_BSTR;
		Command[CurrentCmdIndex--].bstrVal = SysAllocString(ValFormat.toStdWString().c_str());
		Command[CurrentCmdIndex].vt = VT_BSTR;
		Command[CurrentCmdIndex--].bstrVal = SysAllocString(ColorString.toStdWString().c_str());
		Params.rgdispidNamedArgs = NULL;
		Params.rgvarg=Command;
		Params.cArgs = 6;
		Params.cNamedArgs = 0;
		if(dispid == 0)
		{
			wchar_t *ucName = L"Run";
			hr = ExcelCommons::pExcelDisp->GetIDsOfNames(IID_NULL, &ucName, 1,
				LOCALE_SYSTEM_DEFAULT, &dispid);
			if(FAILED(hr))
			{
				SysFreeString(Params.rgvarg[Params.cArgs-1].bstrVal);
				SysFreeString(Params.rgvarg[Params.cArgs-2].bstrVal);
				SysFreeString(Params.rgvarg[Params.cArgs-4].bstrVal);
				SysFreeString(Params.rgvarg[Params.cArgs-5].bstrVal);
				SysFreeString(Params.rgvarg[Params.cArgs-6].bstrVal);
				return hr;
			}
		}
		hr = ExcelCommons::pExcelDisp->Invoke(dispid,IID_NULL,LOCALE_SYSTEM_DEFAULT,
			DISPATCH_METHOD, &Params, NULL, NULL, NULL);
		if(FAILED(hr))
		{
			SysFreeString(Params.rgvarg[Params.cArgs-1].bstrVal);
			SysFreeString(Params.rgvarg[Params.cArgs-2].bstrVal);
			SysFreeString(Params.rgvarg[Params.cArgs-4].bstrVal);
			SysFreeString(Params.rgvarg[Params.cArgs-5].bstrVal);
			SysFreeString(Params.rgvarg[Params.cArgs-6].bstrVal);
			return hr;
		}
	}
	catch(_com_error &ce)
	{
		hr = ce.Error();
	}
	SysFreeString(Params.rgvarg[Params.cArgs-1].bstrVal);
	SysFreeString(Params.rgvarg[Params.cArgs-2].bstrVal);
	SysFreeString(Params.rgvarg[Params.cArgs-4].bstrVal);
	SysFreeString(Params.rgvarg[Params.cArgs-5].bstrVal);
	SysFreeString(Params.rgvarg[Params.cArgs-6].bstrVal);
	return hr;
}
HRESULT ExcelOutput::PrintDataColumn(const QList<double>& Values ,const QString& TargetCell,const QString& ValFormat){
	ExcelCommons::InitExcelOLE();
	SAFEARRAYBOUND  Bound;
	Bound.lLbound   = 1;
	Bound.cElements = Values.size();
	SAFEARRAY* saData;
	saData = SafeArrayCreate(VT_VARIANT, 1, &Bound);
	VARIANT HUGEP *pdFreq;
	HRESULT hr = SafeArrayAccessData(saData, (void HUGEP* FAR*)&pdFreq);
	if (SUCCEEDED(hr))
	{
		foreach(const double& singleValue,Values){
			pdFreq->vt = VT_R8;
			pdFreq->dblVal = singleValue;
			pdFreq++;
		}
		SafeArrayUnaccessData(saData);
	}
	static DISPID dispid = 0;
	DISPPARAMS Params;
	VARIANTARG Command[4];
	int CurrentCmdIndex=4-1;
	if(!ExcelCommons::pExcelDisp)return S_FALSE;
	try
	{
		Command[CurrentCmdIndex].vt = VT_BSTR;
		Command[CurrentCmdIndex--].bstrVal = SysAllocString(L"PrintDataColumn");
		Command[CurrentCmdIndex].vt = VT_ARRAY | VT_VARIANT;
		Command[CurrentCmdIndex--].parray = saData;
		Command[CurrentCmdIndex].vt = VT_BSTR;
		Command[CurrentCmdIndex--].bstrVal = SysAllocString(TargetCell.toStdWString().c_str());
		Command[CurrentCmdIndex].vt = VT_BSTR;
		Command[CurrentCmdIndex--].bstrVal = SysAllocString(ValFormat.toStdWString().c_str());
		Params.rgdispidNamedArgs = NULL;
		Params.rgvarg=Command;
		Params.cArgs = 4;
		Params.cNamedArgs = 0;
		if(dispid == 0)
		{
			wchar_t *ucName = L"Run";
			hr = ExcelCommons::pExcelDisp->GetIDsOfNames(IID_NULL, &ucName, 1,
				LOCALE_SYSTEM_DEFAULT, &dispid);
			if(FAILED(hr))
			{
				SysFreeString(Params.rgvarg[Params.cArgs-1].bstrVal);
				SysFreeString(Params.rgvarg[Params.cArgs-3].bstrVal);
				SysFreeString(Params.rgvarg[Params.cArgs-4].bstrVal);
				return hr;
			}
		}
		hr = ExcelCommons::pExcelDisp->Invoke(dispid,IID_NULL,LOCALE_SYSTEM_DEFAULT,
			DISPATCH_METHOD, &Params, NULL, NULL, NULL);
		if(FAILED(hr))
		{
			SysFreeString(Params.rgvarg[Params.cArgs-1].bstrVal);
			SysFreeString(Params.rgvarg[Params.cArgs-3].bstrVal);
			SysFreeString(Params.rgvarg[Params.cArgs-4].bstrVal);
			return hr;
		}
	}
	catch(_com_error &ce)
	{
		hr = ce.Error();
	}
	SysFreeString(Params.rgvarg[Params.cArgs-1].bstrVal);
				SysFreeString(Params.rgvarg[Params.cArgs-3].bstrVal);
				SysFreeString(Params.rgvarg[Params.cArgs-4].bstrVal);
	return hr;
}