#include "MockSMBClient.h"

bool UMockSMBClient::Connect(const FString& IPAddress, const FString& ShareName, const FString& Username, const FString& Password)
{
	// Simulate connection logic
	bIsConnected = true;
	return true;
}

void UMockSMBClient::Disconnect()
{
	bIsConnected = false;
}

void UMockSMBClient::ListDirectoryAsync(const FString& DirectoryPath)
{
	if (!bIsConnected)
	{
		OnDirectoryListComplete.Broadcast(false, TArray<FSMBEntry>());
		return;
	}

	// Generate some mock entries
	TArray<FSMBEntry> MockEntries;

	if (DirectoryPath == TEXT("") || DirectoryPath == TEXT("/"))
	{
		MockEntries.Add(FSMBEntry(TEXT("Movies"), TEXT("/Movies"), true));
		MockEntries.Add(FSMBEntry(TEXT("VR Experiences"), TEXT("/VR Experiences"), true));
		MockEntries.Add(FSMBEntry(TEXT("sample_video.mp4"), TEXT("/sample_video.mp4"), false));
	}
	else if (DirectoryPath == TEXT("/Movies"))
	{
		MockEntries.Add(FSMBEntry(TEXT(".."), TEXT("/"), true));
		MockEntries.Add(FSMBEntry(TEXT("action_movie_360.mp4"), TEXT("/Movies/action_movie_360.mp4"), false));
		MockEntries.Add(FSMBEntry(TEXT("documentary_180_sbs.mp4"), TEXT("/Movies/documentary_180_sbs.mp4"), false));
	}
	else if (DirectoryPath == TEXT("/VR Experiences"))
	{
		MockEntries.Add(FSMBEntry(TEXT(".."), TEXT("/"), true));
		MockEntries.Add(FSMBEntry(TEXT("rollercoaster_360.mp4"), TEXT("/VR Experiences/rollercoaster_360.mp4"), false));
	}
	else
	{
		// Invalid path
		OnDirectoryListComplete.Broadcast(false, TArray<FSMBEntry>());
		return;
	}

	// Broadcast success
	OnDirectoryListComplete.Broadcast(true, MockEntries);
}

FString UMockSMBClient::GetStreamUrlForFile(const FString& FilePath)
{
	// For mock purposes, just return the path.
	// In a real scenario using an HTTP proxy, this would be:
	// return FString::Printf(TEXT("http://127.0.0.1:8080/stream?path=%s"), *FilePath);

	// Or, if local testing, return a local file path:
	// return TEXT("file:///C:/Users/User/Videos/") + FPaths::GetCleanFilename(FilePath);

	return FilePath;
}
