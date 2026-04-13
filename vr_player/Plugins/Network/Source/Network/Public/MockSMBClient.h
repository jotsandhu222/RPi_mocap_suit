#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SMBClient.h"
#include "MockSMBClient.generated.h"

/**
 * A mock implementation of the ISMBClient interface for testing the UI and workflow
 * without needing a real SMB server setup.
 */
UCLASS(Blueprintable)
class NETWORK_API UMockSMBClient : public UObject, public ISMBClient
{
	GENERATED_BODY()

public:
	//~ Begin ISMBClient Interface
	virtual bool Connect(const FString& IPAddress, const FString& ShareName, const FString& Username, const FString& Password) override;
	virtual void Disconnect() override;
	virtual void ListDirectoryAsync(const FString& DirectoryPath) override;
	virtual FString GetStreamUrlForFile(const FString& FilePath) override;
	//~ End ISMBClient Interface

	/** Delegate fired when the mock directory listing is complete */
	UPROPERTY(BlueprintAssignable, Category = "SMB")
	FOnSMBDirectoryListComplete OnDirectoryListComplete;

private:
	bool bIsConnected = false;
};
