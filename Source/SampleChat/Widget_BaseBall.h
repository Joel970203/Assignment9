#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/EditableTextBox.h"
#include "Widget_BaseBall.generated.h"

UCLASS()
class SAMPLECHAT_API UWidget_BaseBall : public UUserWidget
{
    GENERATED_BODY()

public:
    FString LastSentMessage;
    UPROPERTY()
    bool bPendingClearText = false;

    UPROPERTY(meta = (BindWidget))
    class UEditableTextBox* EditableTextBox_91;

    UPROPERTY(meta = (BindWidget))
    class UEditableTextBox* EditableTextBox_92;

    UFUNCTION(BlueprintCallable, Category = "Chat")
    void OnEnterPressed(const FString& Message);

    void SetResultInEditableTextBox(const FString& Result);

protected:
    virtual void NativeConstruct() override;

    UFUNCTION()
    void OnTextCommitted(const FText& Text, ETextCommit::Type CommitType);
};