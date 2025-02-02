root.TitleMenu = {
    Type = TitleMenuType.CHLCC,
    DrawType = DrawComponentType.SystemMenu,
    PressToStartX = 72,
    PressToStartY = 595,
    PressToStartAnimDurationIn = 0.5,
    PressToStartAnimDurationOut = 0.5,
    PressToStartSprite = "TitleMenuPressToStart",
    BackgroundSprite = "TitleMenuBackground",
    IntroBackgroundSprite = "TitleMenuIntroBackground",
    IntroPanningAnimationDuration = 2.1,
    IntroAfterPanningWaitDuration = 0.8,
    IntroBouncingStarSprite = "StarLogo",
    IntroBouncingStarAnimationDuration = 3.73;
    IntroExplodingStarSprite = "IntroSmallStar",
    IntroExplodingStarAnimationDuration = 1.067,
    IntroExplodingStarAnimationRotationDuration = 0.5,
    IntroExplodingStarAnimationDistance = 294,
    IntroFallingStarSprite = "IntroBigStar",
    IntroFallingStarsAnimationDuration = 3,
    IntroFallingStarsAnimationDistance = 2546,
    IntroFallingStarsAnimationDirection = { X = -1, Y = 1 },
    IntroFallingStarsAnimationRotationDuration = 0.7,
    DelusionADVUnderSprite = "DelusionADVUnder", -- "DelusionADVUnderEnglish" with the TLed assets, "DelusionADVUnder" with the original ones
    DelusionADVUnderX = 78, --74 with the TLed assets, 78 with the original ones
    DelusionADVUnderY = 394, --396 with the TLed assets, 394 with the original ones
    DelusionADVSprite = "DelusionADV", -- "DelusionADVEnglish" with the TLed assets, "DelusionADV" with the original ones
    DelusionADVX = 78, --74 with the TLed assets, 78 with the original ones
    DelusionADVY = 394, --396 with the TLed assets, 394 with the original ones
    SeiraUnderSprite = "SeiraUnder",
    SeiraUnderX = 733,
    SeiraUnderY = 0,
    SeiraSprite = "Seira",
    SeiraX = 728,
    SeiraY = -47,
    CHLogoSprite = "CHLogo",
    CHLogoX = 61,
    CHLogoY = 279,
    LCCLogoUnderSprite = "LCCLogoUnder",
    LCCLogoUnderX = 241,
    LCCLogoUnderY = 327,
    ChuLeftLogoSprite = "ChuLeftLogo",
    ChuLeftLogoX = 353,
    ChuLeftLogoY = 336,
    ChuRightLogoSprite = "ChuRightLogo",
    ChuRightLogoX = 500,
    ChuRightLogoY = 316,
    LoveLogoSprite = "LoveLogo",
    LoveLogoX = 235, --231 with the TLed assets, 235 with the original ones
    LoveLogoY = 336, --335 with the TLed assets, 336 with the original ones
    StarLogoSprite = "StarLogo",
    StarLogoX = 465,
    StarLogoY = 316,
    ExclMarkLogoSprite = "ExclMarkLogo",
    ExclMarkLogoX = 614,
    ExclMarkLogoY = 316,
    CopyrightTextSprite = "CopyrightText",
    CopyrightTextX = 72,
    CopyrightTextY = 675,
    SpinningCircleSprite = "SpinningCircle",
    SpinningCircleX = 610.5,
    SpinningCircleY = -285.5,
    SpinningCircleAnimationDuration = 15,
    ItemHighlightSprite = "TitleMenuItemHighlight",
    ItemHighlightOffsetX = 73,
    ItemHighlightOffsetY = 7,
    ItemPadding = 40,
    ItemYBase = 69,
    ItemFadeInDuration = 0.3,
    ItemFadeOutDuration = 0.6,
    SecondaryItemFadeInDuration = 0.2,
    SecondaryItemFadeOutDuration = 0.2,
    PrimaryFadeInDuration = 0.3,
    PrimaryFadeOutDuration = 0.3,
    SecondaryFadeInDuration = 0.512,
    SecondaryFadeOutDuration = 0.512,
    ItemHyperUpLine = "TitleMenuItemHyperUpLine",
    ItemSuperUpLine = "TitleMenuItemSuperUpLine",
    ItemUpLine = "TitleMenuItemUpLine",
    ItemStraightLine = "TitleMenuItemStraightLine",
    ItemDownLine = "TitleMenuItemDownLine",
    ItemSuperDownLine = "TitleMenuItemSuperDownLine",
    ItemLoadQuickSprite = "TitleMenuItemLoadQuick",
    SecondaryItemX = 320,
    ItemLoadY = 109,
    ItemLoadQuickY = 83,
    ItemLoadSprite = "TitleMenuItemLoad",
    ItemLoadQuickHighlightedSprite = "TitleMenuItemLoadQuickHighlighted",
    ItemLoadHighlightedSprite = "TitleMenuItemLoadHighlighted",
    SecondaryItemHighlightSprite = "TitleMenuSecondaryItemHighlight",
    ItemClearListY = 71,
    ItemCGLibraryY = 97,
    ItemSoundLibraryY = 123,
    ItemMovieLibraryY = 149,
    ItemTipsY = 175,
    ItemTrophyY = 201,
    ItemConfigY = 163,
    ItemSystemSaveY = 189,
    SecondaryItemHighlightX = 286,
    SecondaryMenuPaddingY = 26,
    SecondaryMenuLoadOffsetY = 76,
    SecondaryMenuLineX = 241,
    SecondaryMenuLoadLineY = 93,
    SecondaryMenuLoadQuickLineY = 119,
    SecondaryMenuExtraClearY = 81,
    SecondaryMenuExtraCGY = 107,
    SecondaryMenuExtraSoundY = 133,
    SecondaryMenuExtraMovieY = 159,
    SecondaryMenuExtraTipsY = 159,
    SecondaryMenuExtraTrophyY = 159,
    SecondaryMenuSystemConfigY = 173,
    SecondaryMenuSystemSaveY = 199,
    MenuEntriesNum = 14,
    MenuEntriesSprites = {},
    MenuEntriesHighlightedSprites = {},
    LineNum = 6,
    LineEntriesSprites = {}
};

root.TitleMenu.IntroHighlightSprites = {
    "IntroBrightGreenHighlight",
    "IntroSunHighlight",
    "IntroGrayHighlight",
    "IntroCrescentRainbowHighlight",
    "IntroBlueHighlight",
    "IntroWhiteHighlight",
    "IntroBrownHighlight",
    "IntroDiamondHighlight",
    "IntroDarkGreenHighlight",
    "IntroCircularRainbowHighlight"
};

-- Positions along the diagonal normalized between -1 and 1
root.TitleMenu.IntroHighlightPositions = {
    -1.13, -1.00, -0.49, 0.00, 0.17,
    0.30, 0.58, 0.69, 0.91, 1.12
};

for i = 0, 3 do
    root.Sprites["TitleMenuEntry" .. i] = {
        Sheet = "Title",
        Bounds = {
            X = 1151,
            Y = 101 + i * 25,
            Width = 188,
            Height = 23
        }
    };
    root.TitleMenu.MenuEntriesSprites[#root.TitleMenu.MenuEntriesSprites + 1] = "TitleMenuEntry" .. i;
end

for i = 0, 9 do
    root.Sprites["TitleMenuEntry" .. (i + 4)] = {
        Sheet = "Title",
        Bounds = {
            X = 1369,
            Y = 684 + i * 22,
            Width = 216,
            Height = 20
        }
    };
    root.TitleMenu.MenuEntriesSprites[#root.TitleMenu.MenuEntriesSprites + 1] = "TitleMenuEntry" .. (i + 4);
end

for i = 0, 3 do
    root.Sprites["TitleMenuEntryHighlighted" .. i] = {
        Sheet = "Title",
        Bounds = {
            X = 1151,
            Y = 1 + i * 25,
            Width = 188,
            Height = 23
        }
    };
    root.TitleMenu.MenuEntriesHighlightedSprites[#root.TitleMenu.MenuEntriesHighlightedSprites + 1] = "TitleMenuEntryHighlighted" .. i;
end

for i = 0, 9 do
    root.Sprites["TitleMenuEntryHighlighted" .. (i + 4)] = {
        Sheet = "Title",
        Bounds = {
            X = 1151,
            Y = 684 + i * 22,
            Width = 216,
            Height = 20
        }
    };
    root.TitleMenu.MenuEntriesHighlightedSprites[#root.TitleMenu.MenuEntriesHighlightedSprites + 1] = "TitleMenuEntryHighlighted" .. (i + 4);
end

root.Sprites["TitleMenuPressToStart"] = {
    Sheet = "Title",
    Bounds = { X = 1, Y = 921, Width = 313, Height = 28 },
};

root.Sprites["DelusionADVUnder"] = {
    Sheet = "Title",
    Bounds = { X = 1863, Y = 772, Width = 163, Height = 27 },
};

root.Sprites["DelusionADVUnderEnglish"] = {
    Sheet = "Title",
    Bounds = { X = 1862, Y = 785, Width = 157, Height = 37 },
};

root.Sprites["DelusionADV"] = {
    Sheet = "Title",
    Bounds = { X = 1863, Y = 728, Width = 163, Height = 27 },
};

root.Sprites["DelusionADVEnglish"] = {
    Sheet = "Title",
    Bounds = { X = 1862, Y = 734, Width = 153, Height = 33 },
};

root.Sprites["SeiraUnder"] = {
    Sheet = "Title",
    Bounds = { X = 555, Y = 1, Width = 594, Height = 768 },
};

root.Sprites["Seira"] = {
    Sheet = "Title",
    Bounds = { X = 1, Y = 1, Width = 552, Height = 768 },
};

root.Sprites["CHLogo"] = {
    Sheet = "Title",
    Bounds = { X = 1, Y = 771, Width = 594, Height = 115 },
};

root.Sprites["LCCLogoUnder"] = {
    Sheet = "Title",
    Bounds = { X = 597, Y = 771, Width = 462, Height = 122 },
};

root.Sprites["ChuLeftLogo"] = {
    Sheet = "Title",
    Bounds = { X = 483, Y = 915, Width = 136, Height = 108 },
};

root.Sprites["ChuRightLogo"] = {
    Sheet = "Title",
    Bounds = { X = 693, Y = 895, Width = 136, Height = 128 },
};

root.Sprites["LoveLogo"] = {
    Sheet = "Title",
    Bounds = { X = 341, Y = 915, Width = 140, Height = 108 },
};

root.Sprites["StarLogo"] = {
    Sheet = "Title",
    Bounds = { X = 621, Y = 895, Width = 70, Height = 128 },
};

root.Sprites["ExclMarkLogo"] = {
    Sheet = "Title",
    Bounds = { X = 831, Y = 895, Width = 82, Height = 128 },
};

root.Sprites["CopyrightText"] = {
    Sheet = "Title",
    Bounds = { X = 193, Y = 891, Width = 380, Height = 24 },
};

root.Sprites["SpinningCircle"] = {
    Sheet = "Title",
    Bounds = { X = 1366, Y = 1, Width = 681, Height = 681 },
};

root.Sprites["TitleMenuIntroBackground"] = {
    Sheet = "TitleBg1",
    Bounds = { X = 0, Y = 0, Width = 1280, Height = 720 },
};

root.Sprites["IntroSmallStar"] = {
    Sheet = "Title",
    Bounds = { X = 1153, Y = 534, Width = 45, Height = 44 },
};

root.Sprites["IntroBigStar"] = {
    Sheet = "Title",
    Bounds = { X = 1156, Y = 345, Width = 178, Height = 170 },
};

root.Sprites["IntroBrightGreenHighlight"] = {
    Sheet = "Highlights",
    Bounds = { X = 1536, Y = 0, Width = 256, Height = 256 },
};

root.Sprites["IntroSunHighlight"] = {
    Sheet = "Highlights",
    Bounds = { X = 0, Y = 0, Width = 512, Height = 512 },
};

root.Sprites["IntroGrayHighlight"] = {
    Sheet = "Highlights",
    Bounds = { X = 1536, Y = 256, Width = 256, Height = 256 },
};

root.Sprites["IntroCrescentRainbowHighlight"] = {
    Sheet = "Highlights",
    Bounds = { X = 0, Y = 512, Width = 512, Height = 512 },
};

root.Sprites["IntroBlueHighlight"] = {
    Sheet = "Highlights",
    Bounds = { X = 1280, Y = 256, Width = 256, Height = 256 },
};

root.Sprites["IntroWhiteHighlight"] = {
    Sheet = "Highlights",
    Bounds = { X = 1280, Y = 0, Width = 256, Height = 256 },
};

root.Sprites["IntroBrownHighlight"] = {
    Sheet = "Highlights",
    Bounds = { X = 1024, Y = 256, Width = 256, Height = 256 },
};

root.Sprites["IntroDiamondHighlight"] = {
    Sheet = "Highlights",
    Bounds = { X = 512, Y = 512, Width = 512, Height = 512 },
};

root.Sprites["IntroDarkGreenHighlight"] = {
    Sheet = "Highlights",
    Bounds = { X = 1024, Y = 0, Width = 256, Height = 256 },
};

root.Sprites["IntroCircularRainbowHighlight"] = {
    Sheet = "Highlights",
    Bounds = { X = 512, Y = 0, Width = 512, Height = 512 },
};

root.Sprites["TitleMenuBackground"] = {
    Sheet = "TitleBg2",
    Bounds = { X = 0, Y = 0, Width = 1280, Height = 720 },
};

root.Sprites["TitleMenuItemHighlight"] = {
    Sheet = "Title",
    Bounds = { X = 915, Y = 951, Width = 244, Height = 36 },
};

root.Sprites["TitleMenuItemHyperUpLine"] = {
    Sheet = "Title",
    Bounds = { X = 1805, Y = 686, Width = 51, Height = 80 },
};

root.Sprites["TitleMenuItemSuperUpLine"] = {
    Sheet = "Title",
    Bounds = { X = 1805, Y = 776, Width = 51, Height = 54 },
};

root.Sprites["TitleMenuItemUpLine"] = {
    Sheet = "Title",
    Bounds = { X = 1805, Y = 845, Width = 51, Height = 28 },
};

root.Sprites["TitleMenuItemStraightLine"] = {
    Sheet = "Title",
    Bounds = { X = 1805, Y = 888, Width = 51, Height = 2 },
};

root.Sprites["TitleMenuItemDownLine"] = {
    Sheet = "Title",
    Bounds = { X = 1805, Y = 910, Width = 51, Height = 28 },
};

root.Sprites["TitleMenuItemSuperDownLine"] = {
    Sheet = "Title",
    Bounds = { X = 1805, Y = 959, Width = 51, Height = 54 },
};

root.TitleMenu.LineEntriesSprites[#root.TitleMenu.LineEntriesSprites + 1] = "TitleMenuItemHyperUpLine";
root.TitleMenu.LineEntriesSprites[#root.TitleMenu.LineEntriesSprites + 1] = "TitleMenuItemSuperUpLine";
root.TitleMenu.LineEntriesSprites[#root.TitleMenu.LineEntriesSprites + 1] = "TitleMenuItemUpLine";
root.TitleMenu.LineEntriesSprites[#root.TitleMenu.LineEntriesSprites + 1] = "TitleMenuItemStraightLine";
root.TitleMenu.LineEntriesSprites[#root.TitleMenu.LineEntriesSprites + 1] = "TitleMenuItemDownLine";
root.TitleMenu.LineEntriesSprites[#root.TitleMenu.LineEntriesSprites + 1] = "TitleMenuItemSuperDownLine";

root.Sprites["TitleMenuItemLoadQuick"] = {
    Sheet = "Title",
    Bounds = { X = 1369, Y = 684, Width = 216, Height = 20 },
};

root.Sprites["TitleMenuItemLoad"] = {
    Sheet = "Title",
    Bounds = { X = 1369, Y = 706, Width = 216, Height = 20 },
};

root.Sprites["TitleMenuItemLoadQuickHighlighted"] = {
    Sheet = "Title",
    Bounds = { X = 1151, Y = 684, Width = 216, Height = 20 },
};

root.Sprites["TitleMenuItemLoadHighlighted"] = {
    Sheet = "Title",
    Bounds = { X = 1151, Y = 706, Width = 216, Height = 20 },
};

root.Sprites["TitleMenuSecondaryItemHighlight"] = {
    Sheet = "Title",
    Bounds = { X = 915, Y = 989, Width = 285, Height = 34 },
};