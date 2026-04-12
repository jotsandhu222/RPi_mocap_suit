import { useState, useEffect } from 'react';
import axios from 'axios';

export default function Login({ onLogin }) {
  const [pin, setPin] = useState('');
  const [error, setError] = useState('');
  const [isFirstSetup, setIsFirstSetup] = useState(false);

  useEffect(() => {
    // Check if system is uninitialized by trying to get pairing info
    axios.get('/api/v1/auth/pairing-info')
      .then(() => setIsFirstSetup(true))
      .catch((err) => {
        if(err.response?.status === 403) setIsFirstSetup(false);
      });
  }, []);

  const handleSubmit = async (e) => {
    e.preventDefault();
    try {
      const response = await axios.post('/api/v1/auth/login', { pin });
      onLogin(response.data.access_token);
    } catch (err) {
      setError(isFirstSetup ? 'Failed to create PIN' : 'Invalid PIN');
    }
  };

  return (
    <div className="min-h-screen flex items-center justify-center bg-gray-50 py-12 px-4 sm:px-6 lg:px-8">
      <div className="max-w-md w-full space-y-8 bg-white p-8 rounded-xl shadow-md">
        <div>
          <h2 className="mt-6 text-center text-3xl font-extrabold text-gray-900">
            {isFirstSetup ? 'Set up Admin PIN' : 'Login to Personal Cloud'}
          </h2>
        </div>
        <form className="mt-8 space-y-6" onSubmit={handleSubmit}>
          {error && <div className="text-red-500 text-center">{error}</div>}
          <div>
            <label htmlFor="pin" className="sr-only">PIN</label>
            <input
              id="pin"
              name="pin"
              type="password"
              required
              className="appearance-none rounded-md relative block w-full px-3 py-2 border border-gray-300 placeholder-gray-500 text-gray-900 focus:outline-none focus:ring-indigo-500 focus:border-indigo-500 focus:z-10 sm:text-sm"
              placeholder="Enter PIN"
              value={pin}
              onChange={(e) => setPin(e.target.value)}
            />
          </div>
          <div>
            <button
              type="submit"
              className="group relative w-full flex justify-center py-2 px-4 border border-transparent text-sm font-medium rounded-md text-white bg-indigo-600 hover:bg-indigo-700 focus:outline-none focus:ring-2 focus:ring-offset-2 focus:ring-indigo-500"
            >
              {isFirstSetup ? 'Create PIN' : 'Sign in'}
            </button>
          </div>
        </form>
      </div>
    </div>
  );
}
