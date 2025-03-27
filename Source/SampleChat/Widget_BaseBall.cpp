#include "Widget_BaseBall.h"
#include "BaseBallController.h"

void UWidget_BaseBall::NativeConstruct()
{
    Super::NativeConstruct();

    if (EditableTextBox_91)
    {
        EditableTextBox_91->OnTextCommitted.AddDynamic(this, &UWidget_BaseBall::OnTextCommitted);
    }
}

void UWidget_BaseBall::OnEnterPressed(const FString& Message)
{
    ABaseBallController* PC = Cast<ABaseBallController>(GetOwningPlayer());
    if (PC)
    {
        PC->OnEnterPressed(Message);
    }

    LastSentMessage = Message;

    if (EditableTextBox_91)
    {
        EditableTextBox_91->SetText(FText::GetEmpty());
    }
}

void UWidget_BaseBall::OnTextCommitted(const FText& Text, ETextCommit::Type CommitType)
{
    if (CommitType == ETextCommit::OnEnter)
    {
        FString Message = Text.ToString();
        UE_LOG(LogTemp, Warning, TEXT("Entered message: %s"), *Message);
        OnEnterPressed(Message);
    }
}

void UWidget_BaseBall::UpdateScoreBoard(int32 GuestWin, int32 HostWin)
{
    if (EditableTextBox_93)
    {
        FString ScoreText = FString::Printf(TEXT("Guest : %d       Host : %d"), GuestWin, HostWin);
        EditableTextBox_93->SetText(FText::FromString(ScoreText));
    }
}

void UWidget_BaseBall::SetResultInEditableTextBox(const FString& Result)
{
    if (EditableTextBox_92)
    {
        EditableTextBox_92->SetText(FText::FromString(Result));
    }
}

void UWidget_BaseBall::UpdateTimerText(int32 Seconds)
{
    if (EditableTextBox_94) 
    {
        FString Text = FString::Printf(TEXT("%d"), Seconds);
        EditableTextBox_94->SetText(FText::FromString(Text));
    }
}
