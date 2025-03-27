#include "BaseBallGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "BaseBallController.h"
#include "Widget_BaseBall.h"

void ABaseBallGameMode::BeginPlay()
{
    Super::BeginPlay();
    AnswerCode = GenerateRandomNumber();

    UE_LOG(LogTemp, Warning, TEXT("[GameMode] New Answer: %d%d%d"), AnswerCode[0], AnswerCode[1], AnswerCode[2]);

    int32 GuestScore = WinCounts.FindRef(TEXT("Guest"));
    int32 HostScore = WinCounts.FindRef(TEXT("Host"));

    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        ABaseBallController* Ctrl = Cast<ABaseBallController>(It->Get());
        if (Ctrl)
        {
            Ctrl->Client_UpdateScore(GuestScore, HostScore);
        }
    }

    CurrentTurnPlayer = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    StartTurnTimer();
}

TArray<int32> ABaseBallGameMode::GenerateRandomNumber()
{
    TArray<int32> Answer;
    while (Answer.Num() < 3)
    {
        int32 Rand = FMath::RandRange(1, 9);
        if (!Answer.Contains(Rand))
        {
            Answer.Add(Rand);
        }
    }
    return Answer;
}

bool ABaseBallGameMode::ParseInputToArray(const FString& Input, TArray<int32>& OutArray)
{
    if (!Input.StartsWith("/")) return false;
    FString Raw = Input.Mid(1);
    if (Raw.Len() != 3) return false;

    OutArray.Empty();
    for (TCHAR Character : Raw)
    {
        if (!FChar::IsDigit(Character)) return false;

        int32 Digit = Character - '0';
        if (Digit < 1 || Digit > 9 || OutArray.Contains(Digit)) return false;

        OutArray.Add(Digit);
    }
    return true;
}

FString ABaseBallGameMode::CalculateResult(const TArray<int32>& Guess)
{
    int32 Strikes = 0, Balls = 0;
    for (int32 i = 0; i < 3; ++i)
    {
        if (Guess[i] == AnswerCode[i])
            Strikes++;
        else if (AnswerCode.Contains(Guess[i]))
            Balls++;
    }

    if (Strikes == 0 && Balls == 0) return TEXT("OUT");
    return FString::Printf(TEXT("%dS%dB"), Strikes, Balls);
}

FString ABaseBallGameMode::ProcessPlayerInput(APlayerController* Player, const FString& Message)
{
    if (FinishedPlayers.Contains(Player)) return TEXT("Player already finished!");

    TArray<int32> Guess;
    if (!ParseInputToArray(Message, Guess))
    {
        TryCounts.FindOrAdd(Player)++;
        if (TryCounts[Player] >= 3)
        {
            FinishedPlayers.Add(Player);
        }
        return TEXT("Invalid Input!");
    }

    FString Result = CalculateResult(Guess);
    TryCounts.FindOrAdd(Player)++;

    return Result;
}

int32 ABaseBallGameMode::GetScore(const FString& InRole) const
{
    return WinCounts.Contains(InRole) ? WinCounts[InRole] : 0;
}

void ABaseBallGameMode::ClearAllClientChatBox()
{
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        ABaseBallController* Ctrl = Cast<ABaseBallController>(It->Get());
        if (Ctrl)
        {
            Ctrl->Client_ClearChatBox();
        }
    }

    ResetGame();
}

void ABaseBallGameMode::ResetGame()
{
    AnswerCode = GenerateRandomNumber();
    TryCounts.Empty();
    FinishedPlayers.Empty();

    CurrentTurnPlayer = UGameplayStatics::GetPlayerController(GetWorld(), 0);

    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        ABaseBallController* Ctrl = Cast<ABaseBallController>(It->Get());
        if (Ctrl)
        {
            Ctrl->Client_ClearChatBox();
        }
    }

    StartTurnTimer();
    UE_LOG(LogTemp, Warning, TEXT("[GameMode] Game Reset. New Answer: %d%d%d"), AnswerCode[0], AnswerCode[1], AnswerCode[2]);
}

void ABaseBallGameMode::StartTurnTimer()
{
    RemainingTime = 10;

    GetWorld()->GetTimerManager().SetTimer(CountdownTimerHandle, this, &ABaseBallGameMode::UpdateCountdown, 1.0f, true);
}

void ABaseBallGameMode::UpdateCountdown()
{
    RemainingTime--;

    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        ABaseBallController* Ctrl = Cast<ABaseBallController>(It->Get());
        if (Ctrl)
        {
            Ctrl->Client_UpdateTimer(RemainingTime);
        }
    }

    if (RemainingTime <= 0 && CurrentTurnPlayer)
    {
        GetWorld()->GetTimerManager().ClearTimer(CountdownTimerHandle);

        FString SenderRole = (CurrentTurnPlayer == UGameplayStatics::GetPlayerController(GetWorld(), 0)) ? TEXT("Host") : TEXT("Guest");
        TryCounts.FindOrAdd(CurrentTurnPlayer)++;

        FString Msg = FString::Printf(TEXT("%s out of time (%d)"), *SenderRole, TryCounts[CurrentTurnPlayer]);

        for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
        {
            ABaseBallController* Ctrl = Cast<ABaseBallController>(It->Get());
            if (Ctrl)
            {
                Ctrl->Client_DisplayResult(TEXT("System"), 0, TEXT(""), Msg);
            }
        }

        if (TryCounts[CurrentTurnPlayer] >= 3)
        {
            FinishedPlayers.Add(CurrentTurnPlayer);
        }

        if (FinishedPlayers.Num() >= 2)
        {
            for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
            {
                ABaseBallController* Ctrl = Cast<ABaseBallController>(It->Get());
                if (Ctrl)
                {
                    Ctrl->Client_DisplayResult(TEXT("System"), 0, TEXT(""), TEXT("DRAW !!  REGAME~"));
                }
            }

            GetWorld()->GetTimerManager().SetTimer(ClearUITimerHandle, this, &ABaseBallGameMode::ClearAllClientChatBox, 2.0f, false);
            return;
        }

        for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
        {
            APlayerController* Other = It->Get();
            if (Other != CurrentTurnPlayer)
            {
                CurrentTurnPlayer = Other;
                break;
            }
        }

        StartTurnTimer();
    }
}

void ABaseBallGameMode::BroadcastMessage(const FString& Msg)
{
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        if (APlayerController* PC = It->Get())
        {
            PC->ClientMessage(Msg);
        }
    }
}

void ABaseBallGameMode::ProcessPlayerInputFromUI(APlayerController* Player, const FString& Message)
{
    if (Player != CurrentTurnPlayer)
    {
        UE_LOG(LogTemp, Warning, TEXT("[GameMode] Ignored input from non-turn player: %s"), *Message);
        return;
    }

    GetWorld()->GetTimerManager().ClearTimer(CountdownTimerHandle);
    FString Result = ProcessPlayerInput(Player, Message);
    int32 TryCount = TryCounts.FindRef(Player);

    FString SenderRole = TEXT("Guest");
    if (Player == UGameplayStatics::GetPlayerController(GetWorld(), 0))
    {
        SenderRole = TEXT("Host");
    }

    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        ABaseBallController* Ctrl = Cast<ABaseBallController>(It->Get());
        if (Ctrl)
        {
            Ctrl->Client_DisplayResult(SenderRole, TryCount, Message, Result);
        }
    }

    if (Result.Contains("3S"))
    {
        WinCounts.FindOrAdd(SenderRole)++;

        int32 HostScore = WinCounts.FindRef(TEXT("Host"));
        int32 GuestScore = WinCounts.FindRef(TEXT("Guest"));

        FString WinMessage = SenderRole + TEXT(" WIN !!  REGAME~");

        for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
        {
            ABaseBallController* Ctrl = Cast<ABaseBallController>(It->Get());
            if (Ctrl)
            {
                Ctrl->Client_DisplayResult(TEXT("System"), 0, TEXT(""), WinMessage);
                Ctrl->Client_UpdateScore(GuestScore, HostScore);
            }
        }

        GetWorld()->GetTimerManager().SetTimer(ClearUITimerHandle, this, &ABaseBallGameMode::ClearAllClientChatBox, 2.0f, false);
        return;
    }

    if (TryCounts[Player] >= 3)
    {
        FinishedPlayers.Add(Player);
    }

    if (FinishedPlayers.Num() >= 2)
    {
        FString DrawMsg = TEXT("DRAW !!  REGAME~");

        for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
        {
            ABaseBallController* Ctrl = Cast<ABaseBallController>(It->Get());
            if (Ctrl)
            {
                Ctrl->Client_DisplayResult(TEXT("System"), 0, TEXT(""), DrawMsg);
            }
        }

        GetWorld()->GetTimerManager().SetTimer(ClearUITimerHandle, this, &ABaseBallGameMode::ClearAllClientChatBox, 2.0f, false);
        return;
    }

    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        APlayerController* OtherPlayer = It->Get();
        if (OtherPlayer != Player)
        {
            CurrentTurnPlayer = OtherPlayer;
            break;
        }
    }

    StartTurnTimer();
}