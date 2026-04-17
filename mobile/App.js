import { NavigationContainer } from '@react-navigation/native';
import { createBottomTabNavigator } from '@react-navigation/bottom-tabs';
import { useState, useEffect } from 'react';
import * as SecureStore from 'expo-secure-store';
import { CameraView, useCameraPermissions } from 'expo-camera';
import { View, Text, Button, TextInput, StyleSheet, ActivityIndicator, Alert } from 'react-native';
import axios from 'axios';
import { Folder, Settings as SettingsIcon } from 'lucide-react-native';
import FilesStack from './src/navigation/FilesStack';

const Tab = createBottomTabNavigator();

function SettingsScreen({ onLogout }) {
  return (
    <View style={styles.container}>
      <Text style={{fontSize: 20, marginBottom: 20}}>Settings</Text>
      <Button title="Disconnect Server" onPress={onLogout} color="red" />
    </View>
  );
}

function PairingScreen({ onPaired }) {
  const [ip, setIp] = useState('');
  const [pin, setPin] = useState('');
  const [scanning, setScanning] = useState(false);
  const [loading, setLoading] = useState(false);
  const [permission, requestPermission] = useCameraPermissions();

  const connectToServer = async (serverIp) => {
    if (!serverIp || !pin) {
      Alert.alert('Error', 'Please enter both IP and PIN');
      return;
    }

    setLoading(true);
    try {
      const response = await axios.post(`http://${serverIp}:8000/api/v1/auth/login`, { pin });
      const token = response.data.access_token;
      onPaired(serverIp, token);
    } catch (err) {
      Alert.alert('Connection Failed', 'Could not connect or Invalid PIN. Ensure you are on the same WiFi.');
    } finally {
      setLoading(false);
    }
  };

  if (!permission) return <View />;

  if (scanning) {
    if (!permission.granted) {
      return (
        <View style={styles.container}>
          <Text style={{ textAlign: 'center', marginBottom: 20 }}>We need your permission to show the camera</Text>
          <Button onPress={requestPermission} title="Grant Permission" />
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
                setIp(parsed.server_ip);
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
      <Text style={{fontSize: 24, fontWeight: 'bold', marginBottom: 30}}>Personal Cloud</Text>

      <View style={styles.card}>
        <TextInput
          style={styles.input}
          placeholder="Server IP (e.g., 192.168.1.50)"
          value={ip}
          onChangeText={setIp}
          autoCapitalize="none"
        />
        <TextInput
          style={styles.input}
          placeholder="Admin PIN"
          value={pin}
          onChangeText={setPin}
          secureTextEntry
          keyboardType="numeric"
        />

        {loading ? (
          <ActivityIndicator size="large" color="#4f46e5" style={{marginVertical: 10}}/>
        ) : (
          <View style={{gap: 15, marginTop: 10}}>
             <Button title="Connect" onPress={() => connectToServer(ip)} />
             <Button title="Scan QR Code for IP" onPress={() => setScanning(true)} color="#6b7280" />
          </View>
        )}
      </View>
    </View>
  );
}

export default function App() {
  const [serverIp, setServerIp] = useState(null);
  const [ready, setReady] = useState(false);

  useEffect(() => {
    SecureStore.getItemAsync('server_ip').then(ip => {
      if (ip) setServerIp(ip);
      setReady(true);
    });
  }, []);

  const handlePair = async (ip, token) => {
    await SecureStore.setItemAsync('server_ip', ip);
    await SecureStore.setItemAsync('auth_token', token);
    setServerIp(ip);
  };

  const handleLogout = async () => {
    await SecureStore.deleteItemAsync('server_ip');
    await SecureStore.deleteItemAsync('auth_token');
    setServerIp(null);
  };

  if (!ready) return <View />;

  if (!serverIp) {
    return <PairingScreen onPaired={handlePair} />;
  }

  return (
    <NavigationContainer>
      <Tab.Navigator
        screenOptions={{
          tabBarActiveTintColor: '#4f46e5',
        }}
      >
        <Tab.Screen
          name="Files"
          component={FilesStack}
          options={{
            tabBarIcon: ({ color, size }) => <Folder color={color} size={size} />,
            headerShown: false
          }}
        />
        <Tab.Screen
          name="Settings"
          options={{
            tabBarIcon: ({ color, size }) => <SettingsIcon color={color} size={size} />
          }}
        >
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
    backgroundColor: '#f3f4f6'
  },
  card: {
    backgroundColor: 'white',
    padding: 20,
    borderRadius: 15,
    width: '100%',
    shadowColor: '#000',
    shadowOffset: { width: 0, height: 2 },
    shadowOpacity: 0.1,
    shadowRadius: 4,
    elevation: 3,
  },
  input: {
    borderWidth: 1,
    borderColor: '#d1d5db',
    padding: 12,
    marginBottom: 15,
    borderRadius: 8,
    fontSize: 16
  }
});
