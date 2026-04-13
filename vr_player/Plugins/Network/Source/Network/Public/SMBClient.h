#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SMBClient.generated.h"

/** Struct representing a file or directory entry from an SMB share */
USTRUCT(BlueprintType)
struct FSMBEntry
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "SMB")
	FString Name;

	UPROPERTY(BlueprintReadOnly, Category = "SMB")
	FString Path;

	UPROPERTY(BlueprintReadOnly, Category = "SMB")
	bool bIsDirectory;

	FSMBEntry() : bIsDirectory(false) {}
	FSMBEntry(const FString& InName, const FString& InPath, bool bInIsDirectory)
		: Name(InName), Path(InPath), bIsDirectory(bInIsDirectory) {}
};

// Delegate for async directory listing
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSMBDirectoryListComplete, bool, bSuccess, const TArray<FSMBEntry>&, Entries);

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType)
class USMBClient : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface abstracting network/SMB access.
 * Allows switching between mock data, libsmb2, or HTTP proxy solutions.
 */
class NETWORK_API ISMBClient
{
	GENERATED_BODY()

public:
	/** Connect to an SMB share using IP, share name, username, and password */
	UFUNCTION(BlueprintCallable, Category = "SMB")
	virtual bool Connect(const FString& IPAddress, const FString& ShareName, const FString& Username, const FString& Password) = 0;

	/** Disconnect from the current SMB share */
	UFUNCTION(BlueprintCallable, Category = "SMB")
	virtual void Disconnect() = 0;

	/** Asynchronously list the contents of a directory on the share */
	UFUNCTION(BlueprintCallable, Category = "SMB")
	virtual void ListDirectoryAsync(const FString& DirectoryPath) = 0;

	/**
	 * Returns a URL string suitable for passing to the Media Framework.
	 * For a mock, this might return a local file path.
	 * For a real implementation, this might return a localhost HTTP proxy URL (e.g. http://127.0.0.1:8080/stream?file=...).
	 */
	UFUNCTION(BlueprintCallable, Category = "SMB")
	virtual FString GetStreamUrlForFile(const FString& FilePath) = 0;
};
