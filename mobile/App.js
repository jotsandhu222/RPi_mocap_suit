import { NavigationContainer } from '@react-navigation/native';
import { createBottomTabNavigator } from '@react-navigation/bottom-tabs';
import { useState, useEffect } from 'react';
import * as SecureStore from 'expo-secure-store';
import { CameraView, useCameraPermissions } from 'expo-camera';
import { View, Text, Button, TextInput, StyleSheet } from 'react-native';

const Tab = createBottomTabNavigator();

// Placeholder screens for mobile scaffolding
function FilesScreen() {
  return (
    <View style={styles.container}>
      <Text>File Browser</Text>
      <Text style={{color: 'gray', marginTop: 10}}>Will connect to local API</Text>
    </View>
  );
}

function SettingsScreen({ onLogout }) {
  return (
    <View style={styles.container}>
      <Text>Settings</Text>
      <View style={{marginTop: 20}}>
        <Button title="Disconnect Server" onPress={onLogout} color="red" />
      </View>
    </View>
  );
}

function PairingScreen({ onPaired }) {
  const [ip, setIp] = useState('');
  const [scanning, setScanning] = useState(false);
  const [permission, requestPermission] = useCameraPermissions();

  if (!permission) {
    return <View />;
  }

  if (scanning) {
    if (!permission.granted) {
      return (
        <View style={styles.container}>
          <Text style={{ textAlign: 'center', marginBottom: 20 }}>We need your permission to show the camera</Text>
          <Button onPress={requestPermission} title="grant permission" />
          <Button title="Cancel" onPress={() => setScanning(false)} />
        </View>
      );
    }

    return (
      <View style={{flex: 1}}>
        <CameraView
          style={{flex: 1}}
          facing="back"
          onBarcodeScanned={({ data }) => {
            try {
              const parsed = JSON.parse(data);
              if (parsed.server_ip) {
                setScanning(false);
                onPaired(parsed.server_ip);
              }
            } catch (e) {
              console.log('Invalid QR code');
            }
          }}
        />
        <Button title="Cancel Scan" onPress={() => setScanning(false)} />
      </View>
    );
  }

  return (
    <View style={styles.container}>
      <Text style={{fontSize: 20, fontWeight: 'bold', marginBottom: 20}}>Connect to Server</Text>
      <TextInput
        style={{borderWidth: 1, borderColor: '#ccc', width: '80%', padding: 10, marginBottom: 20, borderRadius: 5}}
        placeholder="Manual IP (e.g., 192.168.1.50)"
        value={ip}
        onChangeText={setIp}
      />
      <View style={{flexDirection: 'row', gap: 10}}>
         <Button title="Connect" onPress={() => onPaired(ip)} />
         <Button title="Scan QR Code" onPress={() => setScanning(true)} />
      </View>
    </View>
  );
}

export default function App() {
  const [serverIp, setServerIp] = useState(null);

  useEffect(() => {
    SecureStore.getItemAsync('server_ip').then(ip => {
      if (ip) setServerIp(ip);
    });
  }, []);

  const handlePair = async (ip) => {
    await SecureStore.setItemAsync('server_ip', ip);
    setServerIp(ip);
  };

  const handleLogout = async () => {
    await SecureStore.deleteItemAsync('server_ip');
    setServerIp(null);
  };

  if (!serverIp) {
    return <PairingScreen onPaired={handlePair} />;
  }

  return (
    <NavigationContainer>
      <Tab.Navigator>
        <Tab.Screen name="Files" component={FilesScreen} />
        <Tab.Screen name="Settings">
          {(props) => <SettingsScreen {...props} onLogout={handleLogout} />}
        </Tab.Screen>
      </Tab.Navigator>
    </NavigationContainer>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    justifyContent: 'center',
    alignItems: 'center',
    padding: 20,
  },
});
