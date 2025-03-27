#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BaseBallController.generated.h"

UCLASS()
class SAMPLECHAT_API ABaseBallController : public APlayerController
{
    GENERATED_BODY()

public:
    virtual void BeginPlay() override;

    UFUNCTION(Client, Reliable)
    void Client_UpdateScore(int32 GuestWin, int32 HostWin);


    UFUNCTION(Client, Reliable)
    void Client_ClearChatBox();

    UFUNCTION(Server, Reliable)
    void Server_RequestScore();

    UFUNCTION()
    void OnEnterPressed(const FString& Message);

    UFUNCTION(Server, Reliable)
    void Server_SendInputToGameMode(const FString& Message);

    UFUNCTION(Client, Reliable)
    void Client_DisplayResult(const FString& SenderRole, int32 TryCount, const FString& Input, const FString& Result);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<class UWidget_BaseBall> WidgetBaseBallClass;

    UPROPERTY()
    class UWidget_BaseBall* WidgetBaseBall;

    UFUNCTION(Client, Reliable)
    void Client_UpdateTimer(int32 SecondsLeft);

};