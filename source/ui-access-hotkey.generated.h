void updateGuildMenuHotkey(){++polls;
        bool down=key->keyboard->isKeyDown((OIS::KeyCode)clientOptions.bindings[ClientOptions::OpenGuildManagement]);
        if(down && !jWasDown && (!negotiationOpen||mercenarieGameplayUnavailable()) && !gbModalOpen())
        {
            toggleGuildManagement(0);
        }
        jWasDown=down;
}
