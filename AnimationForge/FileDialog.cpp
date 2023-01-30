#include "FileDialog.h"
#include <wrl.h>

#define FLDLGEXCPT(hr) FileDialog::Exception(__LINE__, __FILE__, hr)
#define FLDLGEXCPT_NOTE(hr, note) FileDialog::Exception(__LINE__, __FILE__, hr, note)
#define FLDLG_CHECK(hr) if (FAILED(hr)) { throw FLDLGEXCPT(hr); }

std::vector<std::wstring> FileDialog::OpenFiles(std::vector<FileType> allowed_files, bool multi_file, std::wstring title, std::wstring button, std::wstring label)
{
	using namespace Microsoft::WRL;
	ComPtr<IFileOpenDialog> pDialog = nullptr;
	FLDLG_CHECK(CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_ALL, IID_PPV_ARGS(&pDialog)));
	FILEOPENDIALOGOPTIONS options = 0;
	FLDLG_CHECK(pDialog->GetOptions(&options));
	options |= FOS_FILEMUSTEXIST;
	if (multi_file)
	{
		options |= FOS_ALLOWMULTISELECT;
	}
	FLDLG_CHECK(pDialog->SetOptions(options));
	FLDLG_CHECK(pDialog->SetTitle(title.c_str()));
	FLDLG_CHECK(pDialog->SetOkButtonLabel(button.c_str()));
	FLDLG_CHECK(pDialog->SetFileNameLabel(label.c_str()));
	std::vector<COMDLG_FILTERSPEC> filters;
	filters.resize(allowed_files.size());
	for (int i = 0; i < allowed_files.size(); ++i)
	{
		filters[i].pszName = allowed_files[i].displayName.c_str();
		filters[i].pszSpec = allowed_files[i].filterPattern.c_str();
	}
	FLDLG_CHECK(pDialog->SetFileTypes((UINT)allowed_files.size(), filters.data()));
	FLDLG_CHECK(pDialog->Show(nullptr));
	ComPtr<IShellItemArray> pResults = nullptr;
	FLDLG_CHECK(pDialog->GetResults(&pResults));
	std::vector<std::wstring> files = {};
	DWORD nFiles = 0;
	pResults->GetCount(&nFiles);
	for (unsigned int i = 0; i < nFiles; ++i)
	{
		ComPtr<IShellItem> pResult = nullptr;
		FLDLG_CHECK(pResults->GetItemAt(i, &pResult));
		LPWSTR file = nullptr;
		FLDLG_CHECK(pResult->GetDisplayName(SIGDN_FILESYSPATH, &file));
		files.emplace_back(file);
	}
	return files;
}

std::wstring FileDialog::SaveFile(std::vector<FileType> allowed_files, std::wstring title, std::wstring button, std::wstring label)
{
	using namespace Microsoft::WRL;
	ComPtr<IFileSaveDialog> pDialog = nullptr;
	FLDLG_CHECK(CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_ALL, IID_PPV_ARGS(&pDialog)));
	FLDLG_CHECK(pDialog->SetTitle(title.c_str()));
	FLDLG_CHECK(pDialog->SetOkButtonLabel(button.c_str()));
	FLDLG_CHECK(pDialog->SetFileNameLabel(label.c_str()));
	std::vector<COMDLG_FILTERSPEC> filters;
	filters.resize(allowed_files.size());
	for (int i = 0; i < allowed_files.size(); ++i)
	{
		filters[i].pszName = allowed_files[i].displayName.c_str();
		filters[i].pszSpec = allowed_files[i].filterPattern.c_str();
	}
	FLDLG_CHECK(pDialog->SetFileTypes((UINT)allowed_files.size(), filters.data()));
	FLDLG_CHECK(pDialog->Show(nullptr));
	ComPtr<IShellItem> pResult = nullptr;
	FLDLG_CHECK(pDialog->GetResult(&pResult));
	LPWSTR file = nullptr;
	pResult->GetDisplayName(SIGDN_FILESYSPATH, &file);
	return file;
}

std::wstring FileDialog::GetFolder(std::wstring title, std::wstring button, std::wstring label)
{
	using namespace Microsoft::WRL;
	ComPtr<IFileOpenDialog> pDialog;
	FLDLG_CHECK(CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_ALL, IID_PPV_ARGS(&pDialog)));
	FILEOPENDIALOGOPTIONS options = 0;
	FLDLG_CHECK(pDialog->GetOptions(&options));
	options |= FOS_PICKFOLDERS;
	FLDLG_CHECK(pDialog->SetOptions(options));
	FLDLG_CHECK(pDialog->SetTitle(title.c_str()));
	FLDLG_CHECK(pDialog->SetOkButtonLabel(button.c_str()));
	FLDLG_CHECK(pDialog->SetFileNameLabel(label.c_str()));
	FLDLG_CHECK(pDialog->Show(nullptr));
	ComPtr<IShellItem> pResult = nullptr;
	FLDLG_CHECK(pDialog->GetResult(&pResult));
	LPWSTR folder = nullptr;
	pResult->GetDisplayName(SIGDN_FILESYSPATH, &folder);
	return folder;
}



