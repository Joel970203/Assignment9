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

void ABaseBallGameMode::ProcessPlayerInputFromUI(APlayerController* Player, const FString& Message)
{
    FString Result = ProcessPlayerInput(Player, Message);
    int32 TryCount = TryCounts.FindRef(Player);

    FString SenderRole = TEXT("Guest");
    if (Player == UGameplayStatics::GetPlayerController(GetWorld(), 0))
    {
        SenderRole = TEXT("Host");
    }

    // 클라이언트들에게 결과 전송
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        ABaseBallController* Ctrl = Cast<ABaseBallController>(It->Get());
        if (Ctrl)
        {
            Ctrl->Client_DisplayResult(SenderRole, TryCount, Message, Result);
        }
    }

    // WIN 처리
    if (Result.Contains("3S"))
    {
        FString WinMessage = SenderRole + TEXT(" WIN !!  REGAME~");

        for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
        {
            ABaseBallController* Ctrl = Cast<ABaseBallController>(It->Get());
            if (Ctrl)
            {
                Ctrl->Client_DisplayResult(TEXT("System"), 0, TEXT(""), WinMessage);
            }
        }

        // 2초 뒤에 클리어 + 리셋 진행
        GetWorld()->GetTimerManager().SetTimer(ClearUITimerHandle, this, &ABaseBallGameMode::ClearAllClientChatBox, 2.0f, false);
        return;
    }







    // 실패 처리
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

        // 2초 뒤 클리어 + 리셋
        GetWorld()->GetTimerManager().SetTimer(ClearUITimerHandle, this, &ABaseBallGameMode::ClearAllClientChatBox, 2.0f, false);
        return;
    }




}


void ABaseBallGameMode::ResetGame()
{
    AnswerCode = GenerateRandomNumber();
    TryCounts.Empty();
    FinishedPlayers.Empty();

    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        ABaseBallController* Ctrl = Cast<ABaseBallController>(It->Get());
        if (Ctrl)
        {
            Ctrl->Client_ClearChatBox(); 
        }
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


