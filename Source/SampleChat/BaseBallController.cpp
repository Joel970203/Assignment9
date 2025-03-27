#include "BaseBallController.h"
#include "Kismet/GameplayStatics.h"
#include "BaseBallGameMode.h"
#include "Widget_BaseBall.h"

void ABaseBallController::BeginPlay()
{
    Super::BeginPlay();

    if (IsLocalController() && WidgetBaseBallClass)
    {
        WidgetBaseBall = CreateWidget<UWidget_BaseBall>(this, WidgetBaseBallClass);
        if (WidgetBaseBall)
        {
            WidgetBaseBall->AddToViewport();
            UE_LOG(LogTemp, Warning, TEXT("Widget successfully added to viewport"));

            Server_RequestScore();
        }
    }
}


void ABaseBallController::OnEnterPressed(const FString& Message)
{
    Server_SendInputToGameMode(Message);

    if (WidgetBaseBall && WidgetBaseBall->EditableTextBox_91)
    {
        WidgetBaseBall->EditableTextBox_91->SetText(FText::GetEmpty());
    }
}

void ABaseBallController::Server_SendInputToGameMode_Implementation(const FString& Message)
{
    ABaseBallGameMode* GameMode = Cast<ABaseBallGameMode>(UGameplayStatics::GetGameMode(this));
    if (GameMode)
    {
        GameMode->ProcessPlayerInputFromUI(this, Message);
    }
}

void ABaseBallController::Client_DisplayResult_Implementation(const FString& SenderRole, int32 TryCount, const FString& Input, const FString& Result)
{
    if (!WidgetBaseBall || !WidgetBaseBall->EditableTextBox_92) return;

    if (SenderRole != TEXT("System") && WidgetBaseBall->bPendingClearText)
    {
        WidgetBaseBall->EditableTextBox_92->SetText(FText::GetEmpty());
        WidgetBaseBall->bPendingClearText = false;
    }

    FString NewEntry;

    if (SenderRole == TEXT("System"))
    {
        NewEntry = FString::Printf(TEXT("-> %s\n\n"), *Result);
    }
    else
    {
        FString CleanInput = Input.StartsWith("/") ? Input.Mid(1) : Input;
        NewEntry = FString::Printf(TEXT("%s %d guess : \"%s\"\n-> %s\n\n"), *SenderRole, TryCount, *CleanInput, *Result);
    }

    FText PreviousText = WidgetBaseBall->EditableTextBox_92->GetText();
    FString UpdatedText = PreviousText.ToString() + NewEntry;

    WidgetBaseBall->EditableTextBox_92->SetText(FText::FromString(UpdatedText));
}

void ABaseBallController::Client_ClearChatBox_Implementation()
{
    if (WidgetBaseBall && WidgetBaseBall->EditableTextBox_92)
    {
        WidgetBaseBall->EditableTextBox_92->SetText(FText::GetEmpty());
    }
}

void ABaseBallController::Client_UpdateScore_Implementation(int32 GuestWin, int32 HostWin)
{
    if (WidgetBaseBall)
    {
        WidgetBaseBall->UpdateScoreBoard(GuestWin, HostWin);
    }
}

void ABaseBallController::Server_RequestScore_Implementation()
{
    ABaseBallGameMode* GameMode = Cast<ABaseBallGameMode>(UGameplayStatics::GetGameMode(this));
    if (GameMode)
    {
        int32 GuestScore = GameMode->GetScore(TEXT("Guest"));
        int32 HostScore = GameMode->GetScore(TEXT("Host"));

        Client_UpdateScore(GuestScore, HostScore);
    }
}

void ABaseBallController::Client_UpdateTimer_Implementation(int32 SecondsLeft)
{
    if (WidgetBaseBall)
    {
        WidgetBaseBall->UpdateTimerText(SecondsLeft);
    }
}

