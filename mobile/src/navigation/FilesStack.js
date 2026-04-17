import React from 'react';
import { createNativeStackNavigator } from '@react-navigation/native-stack';
import FilesScreen from '../screens/FilesScreen';
import MediaViewerScreen from '../screens/MediaViewerScreen';

const Stack = createNativeStackNavigator();

export default function FilesStack() {
  return (
    <Stack.Navigator>
      <Stack.Screen
        name="FilesHome"
        component={FilesScreen}
        options={{ headerShown: false }}
      />
      <Stack.Screen
        name="MediaViewer"
        component={MediaViewerScreen}
        options={({ route }) => ({
          title: route.params.file.name,
          headerBackTitleVisible: false,
          headerStyle: { backgroundColor: '#000' },
          headerTintColor: '#fff'
        })}
      />
    </Stack.Navigator>
  );
}
