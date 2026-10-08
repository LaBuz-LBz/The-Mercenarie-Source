    void initialiseTrackerUI()
    {
        loadClientOptions();
        if (!MyGUI::Gui::getInstancePtr()) return;
        MyGUI::Gui* gui = MyGUI::Gui::getInstancePtr();
        launcherEnsureUi();
        if(!trackerIcon){
        trackerIcon = gui->createWidget<MyGUI::Button>(
            "LauncherV9Toggle", 6, 6, 68, 68,
            MyGUI::Align::Default, "Window", "GuildEscortTrackerIcon");
        MercenarieFonts::caption(trackerIcon,"");
        launcherLogo(trackerIcon,10,10,48);
        trackerIcon->eventMouseButtonClick += MyGUI::newDelegate(trackerIconClicked);
        }
        buildMercenarieLauncher(gui);
        trackerIcon->setVisible(true);
    }

