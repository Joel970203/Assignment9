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

void UWidget_BaseBall::SetResultInEditableTextBox(const FString& Result)
{
    if (EditableTextBox_92)
    {
        EditableTextBox_92->SetText(FText::FromString(Result));
    }
}