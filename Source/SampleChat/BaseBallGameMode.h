#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BaseBallGameMode.generated.h"

UCLASS()
class SAMPLECHAT_API ABaseBallGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    virtual void BeginPlay() override;

    FTimerHandle ClearUITimerHandle;

    UFUNCTION()
    void ClearAllClientChatBox();

    UFUNCTION(BlueprintCallable, Category = "BaseBall")
    FString ProcessPlayerInput(APlayerController* Player, const FString& Message);

    UFUNCTION(BlueprintCallable, Category = "BaseBall")
    void ProcessPlayerInputFromUI(APlayerController* Player, const FString& Message);

    void BroadcastMessage(const FString& Msg);

protected:
    TArray<int32> AnswerCode;
    TMap<APlayerController*, int32> TryCounts;
    TSet<APlayerController*> FinishedPlayers;
    bool bShouldClearUI = false;

    void ResetGame();
    TArray<int32> GenerateRandomNumber();
    bool ParseInputToArray(const FString& Input, TArray<int32>& OutArray);
    FString CalculateResult(const TArray<int32>& Guess);
};